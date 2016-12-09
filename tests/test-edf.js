
var edflib = require('./build/Release/edflib');
console.log('EDFLib version ' + edflib.GetVersion('wr') );
edflib.SetLogger(function(s) { console.log(s); } )
var inst = new edflib.Edf();
var fs = require('fs');
var EventEmitter = require('events');

var emitter = new EventEmitter();

try{
	fs.statSync("test_signal_data");
} catch (e) {
	fs.mkdirSync("test_signal_data");
}
var filepath = 'sample_data/Osas2002plusQRS.edf';
var filepath_1 = 'sample_data/1000.edf';

  console.log('file:', new Date());
  ReadFromEdf(filepath_1);

function ReadFromEdf(filepath) {
  inst.OpenRead(filepath, edflib.DO_NOT_READ_ANNOTATIONS, function (err, result) {
    //console.log('OpenRead OK with err ' + err + ' filetype ' + inst.Header().edfsignals);
		var filename = filepath.split('/')[filepath.split('/').length - 1]
		var fileParams = {};
		fileParams.handle = inst.handle;
		fileParams.filetype = inst.filetype;
		fileParams.edfsignals = inst.edfsignals;
		fileParams.file_duration = inst.file_duration;
		fileParams.startdate_day = inst.startdate_day;
		fileParams.startdate_month = inst.startdate_month;
		fileParams.startdate_year = inst.startdate_year;
		fileParams.starttime_subsecond = inst.starttime_subsecond;
		fileParams.starttime_second = inst.starttime_second;
		fileParams.starttime_minute = inst.starttime_minute;
		fileParams.starttime_hour = inst.starttime_hour;
		fileParams.patient = inst.patient;
		fileParams.recording = inst.recording;
		fileParams.patientcode = inst.patientcode;
		fileParams.gender = inst.gender;
		fileParams.birthdate = inst.birthdate;
		fileParams.patient_name = inst.patient_name;
		fileParams.patient_additional = inst.patient_additional;
		fileParams.admincode = inst.admincode;
		fileParams.technician = inst.technician;
		fileParams.equipment = inst.equipment;
		fileParams.recording_additional = inst.recording_additional;
		fileParams.datarecord_duration = inst.datarecord_duration;
		fileParams.datarecords_in_file = inst.datarecords_in_file;
		fileParams.annotations_in_file = inst.annotations_in_file;
		fileParams.signalparam = inst.signalparam;
		console.log(inst.signalparam[0]);
		// Read data from edf - file
		// version of this data format - addon.GetVersion('wr')
		// local patient identification - inst.patient, inst.patientcode, inst.patient_name
		// local recording identification - inst.recording
		// startdate of recording (dd.mm.yy) - inst.startdate_day, inst.startdate_month,	inst.startdate_year
		// starttime of recording (hh.mm.ss) - inst.starttime_hour,inst.starttime_minute, inst.starttime_second,	inst.starttime_subsecond,
		// number of bytes in header record -
		// reserved
		// number of data records (-1 if unknown ) - inst.datarecords_in_file
		// duration of a data record, in seconds - inst.datarecord_duration(not in seconds)
		// number of signals (ns) in data record - inst.edfsignals (without annotations)

		// all signals params - inst.signalparam

		// label (e.g. EEG Fpz-Cz or Body temp) - inst.signalparam[ns].label
		// transducer type (e.g. AgAgCl electrode) - inst.signalparam[ns].transducer
		// physical dimension (e.g. uV or degreeC) - inst.signalparam[ns].physdimension
		// physical minimum (e.g. -500 or 34) - inst.signalparam[ns].phys_max
		// physical maximum (e.g. 500 or 40) - inst.signalparam[ns].phys_min
		// digital minimum (e.g. -2048) - inst.signalparam[ns].dig_max
		// digital maximum (e.g. 2047) - inst.signalparam[ns].dig_max
		// prefiltering (e.g. HP:0.1Hz LP:75Hz) - inst.signalparam[ns].prefilter
		// nr of samples in each data record - inst.signalparam[ns].smp_in_datarecord
		// reserved


    emitter.on('filereaded.' + filename, () => {
      console.log('file readed');
      inst.Close((err, res) => {
        if (err) console.log(err);

        console.log('File closed: ' + res);
        emitter.removeListener('filereaded.' + filename, (res) => {
          console.log(res);
        });
      })
    });

		var readedArr = [];

		emitter.on('readed.full.' + filename, (readedArr) => {
			// TODO: произведем запись нового файла
			console.log(readedArr[0].length);
			console.log(readedArr[1].length);
			CreateNewEdf(inst, fileParams, readedArr);

    });


    for (var paramno in inst.signalparam) {
      var param_val = inst.signalparam[paramno];
			var read_step = 1; // (param_val.smp_in_datarecord * inst.datarecords_in_file > 300) ? param_val.smp_in_datarecord * inst.datarecords_in_file/300 : 1;
			var sample_length = param_val.smp_in_datarecord * inst.datarecords_in_file;

			var readed = 0;
			var readed_data = [];

			ReadSamples(inst, paramno, read_step, sample_length, readed, readed_data, ReadSamplesCallback);

			function ReadSamplesCallback(readed_sample) {
				readedArr.push(readed_sample);

				if (readedArr.length === parseInt(inst.signalparam.length)) {
					emitter.emit('readed.full.' + filename, readedArr);
				}
			}

    }

  });
}

function ReadSamples(inst, signalno, read_step, sample_length, readed,  readed_data, callback) {
	var that = this;

	this.readed = readed || 0;
	this.readed_data = readed_data || [];

	inst.ReadPhysicalSamples(signalno, 1, (err, res) => {
		if (err) return console.error(err);

		this.readed_data.push(res[0]);
		inst.SeekSync(0, this.readed, edflib.SEEK_SET);
		this.readed += read_step;


		if (this.readed <= parseInt(sample_length)) {
			ReadSamples(inst, signalno, read_step, sample_length, this.readed,  this.readed_data, callback);
		} else {
			callback(this.readed_data);
		}
  });

}

function CreateNewEdf(inst, params, readedArr) {
	inst.OpenWrite('sample_data/test.edf', edflib.FILETYPE_EDFPLUS, 2, (err, res) => {
		if (err) console.error(err);

		// Для каждого сигнала необходимо поочереди записывать параметры
		// console.log(readedArr);
		for (var i = 0; i <= params.signalparam.length - 1; i++) {
			console.log('for ', i);
			inst.SetSampleFrequencySync(i, params.signalparam[i].smp_in_datarecord);
			inst.SetPhysicalParametersSync(i, -3000.0, 3000.0);
			inst.SetDigitalParametersSync(i, -32768, 32767);
		}

		console.log('Writing..');
		// var w_data0 = [];
		// var w_data1 = [];
		// for (var a = 0; a < readedArr[0].length; a++) w_data0.push(readedArr[0][a]);
		//
		// for (var b = 0; b < readedArr[1].length; b++) w_data1.push(readedArr[1][b]);
		// console.log(w_data0.length, w_data1.length);
		// for (var u = 0; u < )
		inst.BlockWritePhysicalSamples(readedArr, function(err, result) {
			console.log(err, result);
			// inst.WritePhysicalSamples(w_dara1, function(err, result) {
				if (err) throw err;
				inst.Close(function(err, result) {
					console.log(err, result);
				})
			// });
		});
	})
}
