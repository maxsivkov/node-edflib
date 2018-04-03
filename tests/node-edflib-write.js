var addon = require('../build/Release/edflib');
addon.SetLogger(function(s) { console.log(s); } )

//console.log('EDFLib version ' + addon.GetVersion('wr') );

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



function block_write_samples(inst, a, onc, one) {
    (function(inst, a, onc, one){
        inst.BlockWritePhysicalSamples(a, function (err, result) {
            if (result === "OK") onc(); else one(err === "" ? result : err);
        });
    })(inst, a, onc, one);
}

console.log('addon.FILETYPE_EDFPLUS = ' + addon.FILETYPE_EDFPLUS);
inst.OpenWrite('sample_data/output.edf', addon.FILETYPE_EDFPLUS, 2, function (err, result) {
	console.log('OpenWrite cbk error ' + err + ', result ' + result);
	inst.SetSampleFrequencySync(0, 100);
	inst.SetPhysicalParametersSync(0, -3000.0, 3000.0);
	inst.SetDigitalParametersSync(0, -2000, 2000);
	inst.SetSampleFrequencySync(1, 150);
	inst.SetPhysicalParametersSync(1, -3000.0, 3000.0);
	inst.SetDigitalParametersSync(1, -2000, 2000);
	inst.SetNumberOfAnnotationSignals(64);

	var w_data0 = [], w_data1 = [], bw_data0 = [], bw_data1 = [], bw_data10 = [], bw_data11 = [];
	for(var i = 0; i < 100; i++) w_data0.push(i);
	for(var i = 0; i < 150; i++) w_data1.push(1000 + i);

	for(var i = 0; i < 400; i++) { bw_data0.push(i); bw_data10.push(500 + i); }
	for(var i = 0; i < 600; i++) { bw_data1.push(1000 + i); bw_data11.push(1500 + i); }
	console.log('Writing..');
	inst.patient_name = 'sivkov';
	inst.recording_additional = 'rec.add 01';
	inst.SetLabelSync(0, 'lalabel0');
	inst.SetPhysicalDimensionSync(0, 'physdim 0');
	inst.SetLabelSync(1, 'lalabel1');
	inst.SetPhysicalDimensionSync(1, 'physdim 1');	
    
    block_write_samples(inst, [bw_data0, bw_data1], function () { 
        console.log("block_write_samples - OK");
        block_write_samples(inst, [bw_data10, bw_data11], function () { 
                console.log("block_write_samples2 - OK");
		
            },
            function (err) { 
                console.log("Error in block_write_samples2");
            });
    },
    function (err) { 
        console.log("Error in block_write_samples '" + err + "'");
    });
    /*
	inst.WritePhysicalSamples(w_data0, function (err, result) {
		console.log('cbk_WritePhysicalSamples1 signal # 0 error ' + err + ', result ' + result);

		inst.WritePhysicalSamples(w_data1, function (err, result) {
			console.log('cbk_WritePhysicalSamples1 signal # 1 error ' + err + ', result ' + result);

			inst.BlockWritePhysicalSamples([bw_data0, bw_data1], function (err, result) {
				console.log('cbk_BlockWritePhysicalSamples error ' + err + ', result ' + result);
				inst.WritePhysicalSamples(w_data0, function (err, result) {
					console.log('cbk_WritePhysicalSamples2 signal # 0 error ' + err + ', result ' + result);
					inst.WritePhysicalSamples(w_data1, function (err, result) {
						console.log('cbk_WritePhysicalSamples2 signal # 1 error ' + err + ', result ' + result);
						
					});
				});

			});

		});

	});
	inst.WriteDigitalShortSamples(w_data0, function (err, result) {
		console.log('cbk_WriteDigitalShortSamples signal # 0 error ' + err + ', result ' + result);
		inst.WriteDigitalShortSamples(w_data1, function (err, result) {
			console.log('cbk_WriteDigitalShortSamples signal # 1 error ' + err + ', result ' + result);
						
		});
	});
	*/
	setTimeout(function () {
		console.log('1 sec later...')
		for(var i = 0; i < 1000;i++) {
			console.log("annot # " + i);
			inst.WriteAnnotationSync(0, i, 'annot #' + i);
		}
		console.log("block_write_samples2 - OK annot OK");

		inst.Close(function (err, result) {
			console.log('cbk_Close file error ' + err + ', result ' + result);
		});
	}, 1000)

} );
