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
