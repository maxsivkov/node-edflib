Node.JS bindings for EdfLib
==============================

# Table of Contents
1. [Description](#description)
2. [Connect](#connect)
3. [Methods for reading](#read-mode)
    * [OpenRead](#OpenRead)
    * [ReadPhysicalSamples](# ReadPhysicalSamples)
    * [ReadDigitalSamples](# ReadDigitalSamples)
    * [SeekSync](#SeekSync)
    * [TellSync](#TellSync)
    * [RewindSync](#RewindSync)
    * [Close](#read-Close)
4. [Methods for writing](#write-mode)
    * [OpenWrite](#OpenWrite)
    * [SetSampleFrequencySync](#SetSampleFrequencySync)
    * [SetPhysicalParametersSync](#SetPhysicalParametersSync)
    * [SetDigitalParametersSync](#SetDigitalParametersSync)
    * [SetLabelSync](#SetLabelSync)
    * [SetPrefilterSync](#SetPrefilterSync)
    * [SetTransducerSync](#SetTransducerSync)
    * [SetPhysicalDimensionSync](#SetPhysicalDimensionSync)
    * [SetPhysicalDimensionSync](#SetPhysicalDimensionSync)
    * [WritePhysicalSamples](#WritePhysicalSamples)
    * [BlockWritePhysicalSamples](#BlockWritePhysicalSamples)
    * [WriteDigitalShortSamples](#WriteDigitalShortSamples)
    * [WriteDigitalSamples](#WriteDigitalSamples)
    * [BlockWriteDigitalShortSamples](#BlockWriteDigitalShortSamples)
    * [BlockWriteDigitalSamples](#BlockWriteDigitalSamples)
    * [BlockWriteDigital3ByteSamples](#BlockWriteDigital3ByteSamples)
    * [WriteAnnotationSync](#WriteAnnotationSync)
    * [WriteAnnotationLatin1Sync](#WriteAnnotationLatin1Sync)
    * [SetNumberOfAnnotationSignals](#SetNumberOfAnnotationSignals)
    * [Close](#write-Close)
    
<a name="description"> </a>
## Library description
-----------------------
This library is a wrapper / binding library [EDFLib](http://www.teuniz.net/edflib/) written in C ++ to read / write data to edf / edf + files.

Detailed documentation on the edf / edf + format can be found in [the official specification](http://www.edfplus.info/specs/index.html).

Using the library:
<a name="connect"> </a>
## Connecting

```javascript
  var edflib = require ('./ build / Release / edflib');
  var inst = new edflib.Edf ();
```
<a name="read-mode"> </a>
## Methods for reading
  <a name="OpenRead"> </a>
  * **OpenRead** (filepath, readAnnot, callback (err, result)) - open the file for reading,

    * ***filepath*** - local path to .edf - file

    * ***readAnnot*** - Specifies whether to annotate, takes values:
        * ***edflib.DO_NOT_READ_ANNOTATIONS*** - Do not read the annotations
        * ***edflib.READ_ANNOTATIONS*** - Read annotations
        * ***edflib.READ_ALL_ANNOTATIONS*** - Read all annotations

    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result is the result of reading.
  <a name="ReadPhysicalSamples"> </a>
  * **ReadPhysicalSamples**(signal, n_sample, callback (err, result)) - reading of n samples from the edf signal, into the buffer, the values ​​are converted to physical quantities, for example, microvolts, beats per minute, etc.
    * ***signal*** - signal number
    * ***n_sample*** - the number of saps to read
    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result is the result of reading.
  <a name="ReadDigitalSamples"> </a>
  * **ReadDigitalSamples**(signal, n_sample, callback (err, result)) - reading n samples from the edf-signal, to the buffer, as a result, returns the "raw" values
    * ***signal*** - signal number
    * ***n_sample*** - the number of saps to read
    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result is the result of reading.
  <a name="SeekSync"> </a>
  * **SeekSync**(_ signal_, _offset_, _whence_) - Set the indicator position for the _signal_ signal to _offset_, depending on the _whence_ value). Returns the value of the indicator position.
    * ***signal*** - signal number
    * ***offset*** - offset
    * ***whence*** - determines the position, takes values:
        * ***edflib.SEEK_SET*** - Set the position from the beginning of the file
        * ***edflib.SEEK_CUR*** - Set the position depending on the current (add or subtract _offset_ from the current position)
        * ***edflib.SEEK_END*** - Set the position from the end of the file (_offset_ must be negative)
  <a name="TellSync"> </a>
  * **TellSync**(_ signal_) - Returns the current value of the indicator position for the signal _signal_
    * ***signal*** - signal number
  <a name="RewindSync"> </a>
  * **RewindSync**(_ signal_) - Sets the current value of the indicator position for the signal _signal_ to 0.
    * ***signal*** - signal number
  <a name="read-Close"> </a>
  * **Close**(callback (err, result)) - closing the file,

    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result is the result of the operation.

  
<a name="write-mode"> </a>
## Methods for writing
  <a name="OpenWrite"> </a>
  * **OpenWrite**(_ filepath_, _fileType_, _signalsNo_, callback (_err_, _result_)) - opening a file for writing,

    * ***filepath*** - the local path to the .edf file (if the file already exists, it will be overwritten)

    * ***fileType*** - Type of generated file:
        * ***edflib.FILETYPE_EDF*** - EDF
        * ***edflib.FILETYPE_EDFPLUS*** - EDF +
        * ***edflib.FILETYPE_BDF*** - BDF
        * ***edflib.FILETYPE_BDFPLUS*** - BDF +
    
    * ***signalsNo*** - Number of signals in the file
    
    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result is the result of the operation.
  <a name="SetSampleFrequencySync"> </a>
  * **SetSampleFrequencySync**(_ signalNo_, _samplerate_) - Sets the number of samples for the _signalNo_ signal. The function must be called ** BEFORE IMPROVING RECORDING **
    
    * ***signalNo*** - The serial number of the signal (starting with 0)
    
    * ***samplerate*** - Number of samples per second
  <a name="SetPhysicalParametersSync"> </a>
  * **SetPhysicalParametersSync**(_ signalNo_, _physicalMinimum_, _physicalMaximum_) - Set the physical minimum and maximum for the _signalNo_ signal. The function must be called ** BEFORE IMPROVING RECORDING **
    
    * ***signalNo*** - The serial number of the signal (starting with 0)
    
    * ***physicalMinimum*** - Physical minimum
    
    * ***physicalMaximum*** - Physical maximum
  <a name="SetDigitalParametersSync"> </a>
  * **SetDigitalParametersSync**(_ signalNo_, _digitalMinimum_, _digitalMaximum_) - Set the logical minimum and maximum for signal _signalNo_. The function must be called ** BEFORE IMPROVING RECORDING **
    
    * ***signalNo*** - The serial number of the signal (starting with 0)
    
    * ***digitalMinimum*** - Logical minimum
    
    * ***digitalMaximum*** - Logical maximum
  <a name="SetLabelSync"> </a>
  * **SetLabelSync**(_ signalNo_, _label_) - Set the text description for the _signalNo_ signal. The function must be called ** BEFORE IMPROVING RECORDING **
    
    * ***signalNo*** - The serial number of the signal (starting with 0)
    
    * ***label*** - Text string with the name of the signal
  <a name="SetPrefilterSync"> </a>
  * **SetPrefilterSync**(_ signalNo_, _prefilter_) - Set prefilter (for example "HP: 0.1Hz", "LP: 75Hz N: 50Hz", etc.) for the _signalNo_ signal. The function must be called ** BEFORE IMPROVING RECORDING **
    
    * ***signalNo*** - The serial number of the signal (starting with 0)
    
    * ***prefilter*** - Filter name
  <a name="SetTransducerSync"> </a>
  * **SetTransducerSync** (_ signalNo_, _transducer_) - Set the name of the sensor (for example "AgAgCl cup electrodes,", etc.) for the _signalNo_ signal. The function must be called ** BEFORE IMPROVING RECORDING **
    
    * ***signalNo*** - The serial number of the signal (starting with 0)
    
    * ***transducer*** - Sensor name
  <a name="SetPhysicalDimensionSync"> </a>
  * **SetPhysicalDimensionSync**(_ signalNo_, _dimensions_) - Set the unit of measurement (for example uV "," BPM "," mA "," Degr. ", Etc.) for the _signalNo_ signal. The function must be called ** BEFORE RECORDING **
    
    * ***signalNo*** - The serial number of the signal (starting with 0)
    
    * ***dimensions*** - Units of measurement
  <a name="WritePhysicalSamples"> </a>
  * **WritePhysicalSamples**(_ samplesArray_, callback (_err_, _result_)) - Write an array of _samplesArray_ to the file. The sequence number of the _WritePhysicalSamples_ function call corresponds to the signal number.
            Those. let's say the number of signals = 2, the first call corresponds to the signal No. 0, the second call to the signal No. 1, the third to the signal No. 0, the fourth to the signal No. 1, etc.
    
    * ***samplesArray*** - An array with signal values, its length should be equal to _samplerate_
    
    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result is the result of the operation.
  <a name="BlockWritePhysicalSamples"> </a>
  * **BlockWritePhysicalSamples**(_ arrayOfSamplesArray_, callback (_err_, _result_)) - Write signals to the file in block mode.
    
    * ***arrayOfSamplesArray*** - An array of arrays with signal values, the index of the subarray in the array _arrayOfSamplesArray_ corresponds to the signal number. The number of elements in each subarray _arrayOfSamplesArray_ must be a multiple of _samplerate_
            Those. suppose you want to record the data of two signals (with _samplerate_ 100 and 150) in 3 seconds. In this case, the number of elements in the first array will be 300, in the second 450.
    
    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result is the result of the operation.

<a name="WriteDigitalShortSamples"> </a>
  * **WriteDigitalShortSamples**(_ samplesArray_, callback (_err_, _result_)) Write an array of _samplesArray_ to the file. The sequence number of the _WriteDigitalShortSamples_ function call corresponds to the signal number. The value of each element of the _samplesArray_ array must be an integer and must be between -32768 and 32767.
        
    * ***samplesArray*** - An array with signal values, its length should be equal to _samplerate_
    
    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result is the result of the operation.
  <a name="WriteDigitalSamples"> </a>
  * **WriteDigitalSamples**(_ samplesArray_, callback (_err_, _result_)) Write an array of _samplesArray_ to the file. The sequence number of the _WriteDigitalShortSamples_ function call corresponds to the signal number. The value of each element of the array _samplesArray_ must be an integer
        
    * ***samplesArray*** - An array with signal values, its length should be equal to _samplerate_
    
    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result is the result of the operation.
  <a name="BlockWriteDigitalShortSamples"> </a>
  * **BlockWriteDigitalShortSamples**(_ arrayOfSamplesArray_, callback (_err_, _result_)) - Write signals to the file in block mode.
    
    * ***arrayOfSamplesArray*** - An array of arrays with signal values, the index of the subarray in the array _arrayOfSamplesArray_ corresponds to the signal number. The number of elements in each _arrayOfSamplesArray_ subarray must be a multiple of _samplerate_, the value of each subarray element _arrayOfSamplesArray_ must be an integer and be within -32768 to 32767.
            Those. suppose you want to record the data of two signals (with _samplerate_ 100 and 150) in 3 seconds. In this case, the number of elements in the first array will be 300, in the second 450.
    
    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result - the result of the operation. <a name="write-Close"> </a>
  <a name="BlockWriteDigitalSamples"> </a>
  * **BlockWriteDigitalSamples**(_ arrayOfSamplesArray_, callback (_err_, _result_)) - Write signals to the file in block mode.
    
    * ***arrayOfSamplesArray*** - An array of arrays with signal values, the index of the subarray in the array _arrayOfSamplesArray_ corresponds to the signal number. The number of elements in each subarray _arrayOfSamplesArray_ must be a multiple of _samplerate_, the value of each subarray element _arrayOfSamplesArray_ must be an integer
            Those. suppose you want to record the data of two signals (with _samplerate_ 100 and 150) in 3 seconds. In this case, the number of elements in the first array will be 300, in the second 450.
    
    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result - the result of the operation. <a name="write-Close"> </a>
  <a name="BlockWriteDigital3ByteSamples"> </a>
  * **BlockWriteDigital3ByteSamples**(_ arrayOfSamplesArray_, callback (_err_, _result_)) - Write signals to the file in block mode.
    
    * ***arrayOfSamplesArray*** - An array of arrays with signal values, the index of the subarray in the array _arrayOfSamplesArray_ corresponds to the signal number. The number of elements in each subarray _arrayOfSamplesArray_ must be a multiple of _samplerate_, the value of each subarray element _arrayOfSamplesArray_ must be an integer, the function writes the first 24 bits of the value of each signal.
            Those. suppose you want to record the data of two signals (with _samplerate_ 100 and 150) in 3 seconds. In this case, the number of elements in the first array will be 300, in the second 450.
    
    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result - the result of the operation. <a name="write-Close"> </a>
  <a name="WriteAnnotationSync"> </a>
  * **WriteAnnotationSync**(_ onset_, _duration_, _description_) Write annotations to a file
        
    * ***onset*** - Relative event time (relative to starttime and startdate file), measured in 10,000 seconds (0.0001 seconds)
    
    * ***duration*** - The duration of the event, measured in 10,000 seconds (0.0001 seconds)
    
    * ***description*** - Event description, encoded in UTF8
  <a name="WriteAnnotationLatin1Sync"> </a>
  * **WriteAnnotationLatin1Sync**(_ onset_, _duration_, _description_) Write annotations to a file
        
    * ***onset*** - Relative event time (relative to starttime and startdate file), measured in 10,000 seconds (0.0001 seconds)
    
    * ***duration*** - The duration of the event, measured in 10,000 seconds (0.0001 seconds)
    
    * ***description*** - Event description, in Latin1
<a name="SetNumberOfAnnotationSignals"> </a>
  * **SetNumberOfAnnotationSignals** (number) Set the number of signals for annotations
        
    * ***number*** - Number of signals for annotations. The total number of annotations in a file can not exceed the product of the number of seconds by the number of signals for annotations.
    Those. to record N annotations, you need to set the number of signals for annotations in (N / number_of_seconds + 1)
  * **Close**(callback (err, result)) - closing the file, after calling this function, the file hider and annotations will be written.

    * ***callback*** - callback function, in which two parameters are passed err - in case of an error, result is the result of the operation.

## 4. Properties:
  The property can be in read-only mode **RO**, or in read / write mode **RW**
    
    
  * **handle** - identifier, used to distinguish different open files, **RO**
  * **filetype** - File type, **RO**
  
    * ***edflib.FILETYPE_EDF*** - EDF
    * ***edflib.FILETYPE_EDFPLUS*** - EDF +
    * ***edflib.FILETYPE_BDF*** - BDF
    * ***edflib.FILETYPE_BDFPLUS*** - BDF +
  * **edfsignals** - the number of signals in the file, the annotations are not taken into account, **RO**
  * **file_duration** - the duration of the file, expressed in units of = 100 per ** ns, **RO**
  * **startdate_day** - start date of recording, **RO** in read mode, **RW** in recording mode.
  * **startdate_month** - recording start month, **RO** in read mode, **RW** in recording mode.
  * **startdate_year** - year of recording start, **RO** in read mode, **RW** in recording mode.
  * **starttime_subsecond** - initial offset time, expressed in units of 100 nanoseconds. Always less than 10,000,000 (one second). only edfplus and bdfplus are used, **RO**
  * **starttime_second** - recording start seconds, **RO** in read mode, **RW** in recording mode.
  * **starttime_minute** - minutes of recording start, **RO** in read mode, **RW** in recording mode.
  * **starttime_hour** - hours of recording start, **RO** in read mode, **RW** in recording mode.
  * **patient** - includes patient header field, always empty, when file type is EDF +, **RO**
  * **recording** - includes the header recording field, always empty, when the file type is EDF +, **RO**
  * **patientcode** - patient code, **RO** in read mode, **RW** in write mode.
  * **gender** - patient's gender, **RO** in read mode, **RW** in recording mode.
  * **birthdate** - patient's date of birth, **RO** in read mode, **RW** in recording mode.
  * **patient_name** - patient name, **RO** in read mode, **RW** in recording mode.
  * **patient_additional** - in addition to the patient, **RO** in read mode, **RW** in recording mode.
  * **admincode** - admin code, **RO** in read mode, **RW** in write mode.
  * **technician** - technical field, **RO** in read mode, **RW** in recording mode.
  * **equipment** - equipment, **RO** in read mode, **RW** in recording mode.
  * **recording_additional**, **RO** in read mode, **RW** in recording mode.
  * **datarecord_duration** - The duration of datarecord is expressed in units of 100 nanoseconds, **RO** in read mode, **RW** in write mode.
  * **datarecords_in_file** - the number of datarecords in the file, **RO**
  * **annotations_in_file** - number of annotations in the file, **RO**

  * **signalparam** - an array of JSON objects with information on all signals. Each JSON object includes the following keys, described in the specification for edf / edf +, ** RO **:

    * ***label*** - signal name, string field
    * ***smp_in_file*** - number of samples recorded in the file, numeric field
    * ***phys_max*** - the physical maximum of the signal value, the numeric field
    * ***phys_min*** - the physical minimum of the signal value, the numeric field
    * ***dig_max*** - the logical maximum of the signal value, the numeric field
    * ***dig_min*** - logical minimum of the signal value, numeric field
    * ***smp_in_datarecord*** - the number of signals in the record, numeric field
    * ***physdimension*** - a unit of measurement (for example, uV "," BPM "," mA "," Degr. ", etc.), a string field
    * ***prefilter*** - prefilter (for example "HP: 0.1Hz", "LP: 75Hz N: 50Hz", etc.), the string field
    * ***transducer*** - the name of the sensor (for example "AgAgCl cup electrodes,", etc.), a string field
  * **annotations** - synchronous reading of annotations, returns an array of JSON objects with information on all annotations, ** RO **:
    
    * ***onset*** - Time (relative to starttime) in 100 nanoseconds (0.0001 seconds) of the event, numeric field
    * ***duration*** - duration of the event, string field
    * ***annotation*** - description of the event, string field