var addon = require('./build/Release/edflib');
console.log('EDFLib version ' + addon.GetVersion('wr') );

var inst = new addon.Edf();
var fs = require('fs');

try{
	fs.statSync("test_signal_data");
}catch (e) {
	fs.mkdirSync("test_signal_data");
}

function read_samples(inst, signalno, already_read) {
    (function(inst, signalno, already_read){
        var to_read = 100;
        if ( (already_read + to_read) > inst.signalparam[signalno].smp_in_file) to_read = inst.signalparam[signalno].smp_in_file - already_read;
        if (to_read > 0)
        {
          (function(inst, signalno, already_read) {
            inst.ReadPhysicalSamples(signalno, to_read, function (err, result) {
                fs.appendFileSync('test_signal_data/signal_' + signalno + '.txt', result.join('\n') + '\n');
                //console.log('ReadPhysicalSamples of ' + signalno + ', result ' + result.length + ' to_read ' + to_read + ' already_read ' + already_read);
                read_samples(inst, signalno, already_read);
            })
          })(inst, signalno, already_read + to_read);
        } else console.log("signal " + signalno + " read " + already_read + " samples")
    })(inst, signalno, already_read)
}


inst.OpenRead('sample_data/Osas2002plusQRS.edf', "Read", function (err, result) {
	//console.log('OpenRead OK with err ' + err + ' filetype ' + inst.Header().edfsignals);
	console.log('handle = ' + inst.handle);
	console.log('filetype = ' + inst.filetype);
	console.log('edfsignals = ' + inst.edfsignals);
	console.log('file_duration = ' + inst.file_duration);
	console.log('startdate_day = ' + inst.startdate_day);
	console.log('startdate_month = ' + inst.startdate_month);
	console.log('startdate_year = ' + inst.startdate_year);
	console.log('starttime_subsecond = ' + inst.starttime_subsecond);
	console.log('starttime_second = ' + inst.starttime_second);
	console.log('starttime_minute = ' + inst.starttime_minute);
	console.log('starttime_hour = ' + inst.starttime_hour);
	console.log('patient = ' + inst.patient);
	console.log('recording = ' + inst.recording);
	console.log('patientcode = ' + inst.patientcode);
	console.log('gender = ' + inst.gender);
	console.log('birthdate = ' + inst.birthdate);
	console.log('patient_name = ' + inst.patient_name);
	console.log('patient_additional = ' + inst.patient_additional);
	console.log('admincode = ' + inst.admincode);
	console.log('technician = ' + inst.technician);
	console.log('equipment = ' + inst.equipment);
	console.log('recording_additional = ' + inst.recording_additional);
	console.log('datarecord_duration = ' + inst.datarecord_duration);
	console.log('datarecords_in_file = ' + inst.datarecords_in_file);
	console.log('annotations_in_file = ' + inst.annotations_in_file);
	console.log('signalparam = ' + JSON.stringify(inst.signalparam));
	//console.log(' = ' + inst.);
	console.log('DO_NOT_READ_ANNOTATIONS = ' + addon.DO_NOT_READ_ANNOTATIONS);
	console.log('READ_ANNOTATIONS = ' + addon.READ_ANNOTATIONS);
	console.log('READ_ALL_ANNOTATIONS = ' + addon.READ_ALL_ANNOTATIONS);
	for (var paramno in inst.signalparam) {
		paramval = inst.signalparam[paramno];
		console.log('signal param ' + paramno + ' : ' + JSON.stringify(paramval));
		//if (paramno == 0)
			//inst.ReadPhysicalSamples(paramno, paramval.smp_in_file, (err, result) => { 
			//		console.log('ReadPhysicalSamples , result ' + result.length + "  error '" + err + "'"); 
			//});
        	//read_samples(inst, paramno, 0);
        /*(function(paramno){
            inst.ReadPhysicalSamples(paramno, 10, function (err, result) {
                console.log('ReadPhysicalSamples of ' + paramno + ', result ' + result + ' error ' + err);
            })
        })(paramno)
        */
	}







	inst.ReadPhysicalSamples(0, 10, (err, result) => {
		inst.RewindSync(0);
		console.log('ReadPhysicalSamples (10 samples of signal 0): ' + result + ' error ' + err);
		console.log('Signal 0 position ' + inst.TellSync(0));
		res = inst.SeekSync(0, 10, addon.SEEK_CUR);
		console.log('Signal 0 position +10 CUR (result ' + res + " ) tell " + inst.TellSync(0));
		res = inst.SeekSync(0, 100, addon.SEEK_SET);
		console.log('Signal 0 position 100 SET (result ' + res + " ) tell " + inst.TellSync(0));
		res = inst.SeekSync(0, -10, addon.SEEK_END);
		console.log('Signal 0 position -10 END (result ' + res + " ) tell " + inst.TellSync(0));
		inst.RewindSync(0);
		console.log('Signal 0 Rewind ' + inst.TellSync(0));
	});



    /*console.log("*** Annotations sync");
	for (var annotno in inst.annotations) {
		annotval = inst.annotations[annotno];
		console.log('annotation ' + annotno + ' : ' + JSON.stringify(annotval));
	}
	inst.ReadAnnotations(function (err, result) {
        console.log("*** Annotations Async");
		console.log('ReadAnnotations result' + JSON.stringify(result) + ' error ' + err);
	});
    */

/*
        inst.ReadPhysicalSamples(0, 10, function (err, result) {
		console.log('ReadPhysicalSamples result ' + result+ ' error ' + err);
	});


        inst.ReadDigitalSamples(1, 10, function (err, result) {
		console.log('ReadDigitalSamples result ' + result + ' error ' + err);
	});

*/
	// inst.CloseFile(function (err, result) {
	// 	console.log('CloseFile result ' + result);
	// });

	/*setTimeout(function () {
		console.log('1 sec later...')
	}, 1000)
    */
} );
