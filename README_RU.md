Node.JS bindings for EdfLib
==============================

# Оглавление
1. [Описание](#description)
2. [Подключение](#connect)
3. [Методы для чтения](#read-mode)
    * [OpenRead](#OpenRead)
    * [ReadPhysicalSamples](#ReadPhysicalSamples)
    * [ReadDigitalSamples](#ReadDigitalSamples)
    * [SeekSync](#SeekSync)
    * [TellSync](#TellSync)
    * [RewindSync](#RewindSync)
    * [Close](#read-Close)
4. [Методы для записи](#write-mode)
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
    
<a name="description"></a>
## Описание библиотеки 
-----------------------
Данная библиотека является оберткой/привязкой библиотеки [EDFLib] (http://www.teuniz.net/edflib/), написанной на C++ для чтения/записи данных в файлы формата edf/edf+.

Подробная документация по формату edf/edf+ можно посмотреть в [оффициальной спецификации] (http://www.edfplus.info/specs/index.html).

Использование библиотеки:
<a name="connect"></a>
## Подключение 

```javascript
    var edflib = require('./build/Release/edflib');
    var inst = new edflib.Edf();
```
<a name="read-mode"></a>
## Методы для чтения 
  <a name="OpenRead"></a>
  * **OpenRead**(filepath, readAnnot, callback(err, result) ) - открытие файла для чтения,

    * ***filepath*** - локальный путь к .edf - файлу

    * ***readAnnot*** - Определяет чиать ли аннотации, принимает значения: 
        * ***edflib.DO_NOT_READ_ANNOTATIONS*** - Не читать аннотации
        * ***edflib.READ_ANNOTATIONS*** - Читать аннотации
        * ***edflib.READ_ALL_ANNOTATIONS*** - Читать все аннотации

    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат чтения.
  <a name="ReadPhysicalSamples"></a>
  * **ReadPhysicalSamples**(signal, n_sample, callback(err, result)) - чтение n сэмплов из edf-сигннала, в буффер, значения преобразуются в физические величины, например, микровольт, ударов в минуту, и т. д.
    * ***signal*** - номер сигнала
    * ***n_sample*** - количество сэплов для чтения
    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат чтения.
  <a name="ReadDigitalSamples"></a>
  * **ReadDigitalSamples**(signal, n_sample, callback(err, result)) - чтение n сэмплов из edf-сигннала, в буффер, в результате возвращает "сырые" значения
    * ***signal*** - номер сигнала
    * ***n_sample*** - количество сэплов для чтения
    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат чтения.
  <a name="SeekSync"></a>
  * **SeekSync**(_signal_, _offset_, _whence_) - Установка позиции индикатора для сигнала _signal_ в значение _offset_ в зависимости от значения _whence_). Возвращает значение позиции индикатора.
    * ***signal*** - номер сигнала
    * ***offset*** - смещение
    * ***whence*** - определяет позицию,  принимает значения: 
        * ***edflib.SEEK_SET*** - Установить позицию от начала файла 
        * ***edflib.SEEK_CUR*** - Установить позицию в зависимости от текущей (прибавлят или отнимает _offset_ от текущей позиции)
        * ***edflib.SEEK_END*** - Установить позицию от конца файла (_offset_ должен быть отрицательным)
  <a name="TellSync"></a>
  * **TellSync**(_signal_) - Возвращает текущее значение позиции индикатора для сигнала _signal_
    * ***signal*** - номер сигнала
  <a name="RewindSync"></a>
  * **RewindSync**(_signal_) - Устанавливает текущее значение позиции индикатора для сигнала _signal_ в 0.
    * ***signal*** - номер сигнала
  <a name="read-Close"></a>
  * **Close**(callback(err, result)) - закрытие файла,

    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат операции.

  
<a name="write-mode"></a>
## Методы для записи 
  <a name="OpenWrite"></a>
  * **OpenWrite**(_filepath_, _fileType_, _signalsNo_, callback(_err_, _result_) ) - открытие файла для записи,

    * ***filepath*** - локальный путь к .edf - файлу (если файл уже существует, он будет перезаписан)

    * ***fileType*** - Тип создаваемого файла: 
        * ***edflib.FILETYPE_EDF***     - EDF
        * ***edflib.FILETYPE_EDFPLUS*** - EDF+
        * ***edflib.FILETYPE_BDF***     - BDF
        * ***edflib.FILETYPE_BDFPLUS*** - BDF+
    
    * ***signalsNo*** - Количество сигналов в файле 
    
    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат выполнения операции.
  <a name="SetSampleFrequencySync"></a>
  * **SetSampleFrequencySync**(_signalNo_, _samplerate_) - Установка количества семплов для сигнала _signalNo_. Функция должна быть вызвана **ДО ОСУЩЕСТВЛЕНИЯ ЗАПИСИ** 
    
    * ***signalNo*** - Порядковый номер сигнала (начиная с 0)
    
    * ***samplerate*** - Количество семплов в секунду
  <a name="SetPhysicalParametersSync"></a>
  * **SetPhysicalParametersSync**(_signalNo_, _physicalMinimum_, _physicalMaximum_) - Установка физического минимума и максимума для сигнала _signalNo_. Функция должна быть вызвана **ДО ОСУЩЕСТВЛЕНИЯ ЗАПИСИ** 
    
    * ***signalNo*** - Порядковый номер сигнала (начиная с 0)
    
    * ***physicalMinimum*** - Физический минимум
    
    * ***physicalMaximum*** - Физический максимум
  <a name="SetDigitalParametersSync"></a>  
  * **SetDigitalParametersSync**(_signalNo_, _digitalMinimum_, _digitalMaximum_) - Установка логического минимума и максимума для сигнала _signalNo_. Функция должна быть вызвана **ДО ОСУЩЕСТВЛЕНИЯ ЗАПИСИ** 
    
    * ***signalNo*** - Порядковый номер сигнала (начиная с 0)
    
    * ***digitalMinimum*** - Логический минимум
    
    * ***digitalMaximum*** - Логический максимум
  <a name="SetLabelSync"></a>  
  * **SetLabelSync**(_signalNo_, _label_) - Установка текстового описания для сигнала _signalNo_. Функция должна быть вызвана **ДО ОСУЩЕСТВЛЕНИЯ ЗАПИСИ** 
    
    * ***signalNo*** - Порядковый номер сигнала (начиная с 0)
    
    * ***label*** - Текстовая строка с названием сигнала
  <a name="SetPrefilterSync"></a>
  * **SetPrefilterSync**(_signalNo_, _prefilter_) - Установка префильтра (например "HP:0.1Hz", "LP:75Hz N:50Hz", итд) для сигнала _signalNo_. Функция должна быть вызвана **ДО ОСУЩЕСТВЛЕНИЯ ЗАПИСИ** 
    
    * ***signalNo*** - Порядковый номер сигнала (начиная с 0)
    
    * ***prefilter*** - Название фильтра
  <a name="SetTransducerSync"></a>
  * **SetTransducerSync**(_signalNo_, _transducer_) - Установка названия датчика (например "AgAgCl cup electrodes," , итд) для сигнала _signalNo_. Функция должна быть вызвана **ДО ОСУЩЕСТВЛЕНИЯ ЗАПИСИ** 
    
    * ***signalNo*** - Порядковый номер сигнала (начиная с 0)
    
    * ***transducer*** - Название датчика
  <a name="SetPhysicalDimensionSync"></a>
  * **SetPhysicalDimensionSync**(_signalNo_, _dimensions_) - Установка едениц измерения (например uV", "BPM", "mA", "Degr.", итд) для сигнала _signalNo_. Функция должна быть вызвана **ДО ОСУЩЕСТВЛЕНИЯ ЗАПИСИ** 
    
    * ***signalNo*** - Порядковый номер сигнала (начиная с 0)
    
    * ***dimensions*** - Еденицы измерения
  <a name="WritePhysicalSamples"></a>
  * **WritePhysicalSamples**(_samplesArray_, callback(_err_, _result_)) - Записать в файл массив _samplesArray_. Порядковый номер вызова функции _WritePhysicalSamples_ соответствует номеру сигнала. 
            Т.е. допустим количество сигналов = 2, первый вызов соответствует сигналу № 0, второй - сигналу № 1, третий - сигналу № 0, четвертый - сигналу № 1, итд
    
    * ***samplesArray*** - Массив со значениями сигнала, его длина должна быть равна _samplerate_
    
    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат выполнения операции.
  <a name="BlockWritePhysicalSamples"></a>
  * **BlockWritePhysicalSamples**(_arrayOfSamplesArray_, callback(_err_, _result_)) - Записать в файл сигналы в блочном режиме.
    
    * ***arrayOfSamplesArray*** - Массив массивов со значениями сигнала, индекс подмассива в массиве _arrayOfSamplesArray_ соответствует номеру сигнала. Количество элементов в каждом подмассиве _arrayOfSamplesArray_ должно быть кратно _samplerate_
            Т.е. допустим требуется записать данные двух сигналов ( с _samplerate_ 100 и 150) за 3 секунды. В таком случае число элементов в первом массиве будет = 300, во втором 450.
    
    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат выполнения операции.
  <a name="WriteDigitalShortSamples"></a>
  * **WriteDigitalShortSamples**(_samplesArray_, callback(_err_, _result_)) Записать в файл массив _samplesArray_. Порядковый номер вызова функции _WriteDigitalShortSamples_ соответствует номеру сигнала. Значение каждого элемента массива _samplesArray_ должно быть целым числом и находиться в пределах -32768 до 32767.
        
    * ***samplesArray*** - Массив со значениями сигнала, его длина должна быть равна _samplerate_
    
    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат выполнения операции.
  <a name="WriteDigitalSamples"></a>
  * **WriteDigitalSamples**(_samplesArray_, callback(_err_, _result_)) Записать в файл массив _samplesArray_. Порядковый номер вызова функции _WriteDigitalShortSamples_ соответствует номеру сигнала. Значение каждого элемента массива _samplesArray_ должно быть целым числом
        
    * ***samplesArray*** - Массив со значениями сигнала, его длина должна быть равна _samplerate_
    
    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат выполнения операции.
  <a name="BlockWriteDigitalShortSamples"></a>
  * **BlockWriteDigitalShortSamples**(_arrayOfSamplesArray_, callback(_err_, _result_)) - Записать в файл сигналы в блочном режиме.
    
    * ***arrayOfSamplesArray*** - Массив массивов со значениями сигнала, индекс подмассива в массиве _arrayOfSamplesArray_ соответствует номеру сигнала. Количество элементов в каждом подмассиве _arrayOfSamplesArray_ должно быть кратно _samplerate_, значение каждого элемента подмассива _arrayOfSamplesArray_ должно быть целым числом и находиться в пределах -32768 до 32767.
            Т.е. допустим требуется записать данные двух сигналов ( с _samplerate_ 100 и 150) за 3 секунды. В таком случае число элементов в первом массиве будет = 300, во втором 450.
    
    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат выполнения операции.<a name="write-Close"></a>
  <a name="BlockWriteDigitalSamples"></a>
  * **BlockWriteDigitalSamples**(_arrayOfSamplesArray_, callback(_err_, _result_)) - Записать в файл сигналы в блочном режиме.
    
    * ***arrayOfSamplesArray*** - Массив массивов со значениями сигнала, индекс подмассива в массиве _arrayOfSamplesArray_ соответствует номеру сигнала. Количество элементов в каждом подмассиве _arrayOfSamplesArray_ должно быть кратно _samplerate_, значение каждого элемента подмассива _arrayOfSamplesArray_ должно быть целым числом
            Т.е. допустим требуется записать данные двух сигналов ( с _samplerate_ 100 и 150) за 3 секунды. В таком случае число элементов в первом массиве будет = 300, во втором 450.
    
    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат выполнения операции.<a name="write-Close"></a>
  <a name="BlockWriteDigital3ByteSamples"></a>
  * **BlockWriteDigital3ByteSamples**(_arrayOfSamplesArray_, callback(_err_, _result_)) - Записать в файл сигналы в блочном режиме.
    
    * ***arrayOfSamplesArray*** - Массив массивов со значениями сигнала, индекс подмассива в массиве _arrayOfSamplesArray_ соответствует номеру сигнала. Количество элементов в каждом подмассиве _arrayOfSamplesArray_ должно быть кратно _samplerate_, значение каждого элемента подмассива _arrayOfSamplesArray_ должно быть целым числом, функция записывает первые 24 бита значения каждого сигнала.
            Т.е. допустим требуется записать данные двух сигналов ( с _samplerate_ 100 и 150) за 3 секунды. В таком случае число элементов в первом массиве будет = 300, во втором 450.
    
    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат выполнения операции.<a name="write-Close"></a>
  <a name="WriteAnnotationSync"></a>
  * **WriteAnnotationSync**(_onset_, _duration_, _description_) Записать аннотации в файл
        
    * ***onset*** - Относительное время события (относительно starttime и startdate файла), измеряется в 10000 секунды (0.0001 секунды)
    
    * ***duration*** - Длительность события, измеряется в 10000 секунды (0.0001 секунды)
    
    * ***description*** - Описание события, в кодировке UTF8
  <a name="WriteAnnotationLatin1Sync"></a>
  * **WriteAnnotationLatin1Sync**(_onset_, _duration_, _description_) Записать аннотации в файл
        
    * ***onset*** - Относительное время события (относительно starttime и startdate файла), измеряется в 10000 секунды (0.0001 секунды)
    
    * ***duration*** - Длительность события, измеряется в 10000 секунды (0.0001 секунды)
    
    * ***description*** - Описание события, в кодировке Latin1
  <a name="SetNumberOfAnnotationSignals"></a>
  * **SetNumberOfAnnotationSignals**(number) Установить количество сигналов для аннотаций
        
    * ***number*** - Количество сигналов для аннотаций. Общее число аннотаций в файле не может превышать произведения количества секунд на ноличество сигналов для аннотаций.
    Т.е. для записи N аннотаций, нужно установить количество сигналов для аннотаций в (N / number_of_seconds + 1)
  * **Close**(callback(err, result)) - закрытие файла, после вызова этой функции запишется хидер файла, аннотации.

    * ***callback*** - функция обратного вызова, в которую передаются два параметра err - в случае ошибки, result - результат операции.

## 4. Свойства:
  Свойство может быть в режиме только для чтения **RO**, либо в режиме чтение\запись **RW**
    
    
  * **handle** - идентификатор, используется для отличия разных открытых файлов, **RO**
  * **filetype** - Тип файла, **RO**
  
    * ***edflib.FILETYPE_EDF***     - EDF
    * ***edflib.FILETYPE_EDFPLUS*** - EDF+
    * ***edflib.FILETYPE_BDF***     - BDF
    * ***edflib.FILETYPE_BDFPLUS*** - BDF+
  * **edfsignals** - количество сигналов в файле, аннотации не берутся в счет, **RO**
  * **file_duration** - продолжительность файла, выраженная в единицах = 100 на**носикундам, **RO**
  * **startdate_day** - день начала записи, **RO** в режиме чтения, **RW** в режиме записи.
  * **startdate_month** - месяц начала записи, **RO** в режиме чтения, **RW** в режиме записи.
  * **startdate_year** - год начала записи, **RO** в режиме чтения, **RW** в режиме записи.
  * **starttime_subsecond** - начальное время смещения, выражается в единицах, равных 100 наносекундам. Всегда меньше, чем 10000000 (одна секунда). используется только edfplus и bdfplus, **RO**
  * **starttime_second** - секунды начала записи, **RO** в режиме чтения, **RW** в режиме записи.
  * **starttime_minute** - минуты начала записи, **RO** в режиме чтения, **RW** в режиме записи.
  * **starttime_hour** - часы начала записи, **RO** в режиме чтения, **RW** в режиме записи.
  * **patient** - включает поле patient хедера, всегда пустой, когда тип файла - EDF+, **RO**
  * **recording**- включает поле recording хедера, всегда пустой, когда тип файла - EDF+, **RO**
  * **patientcode** - код пациента, **RO** в режиме чтения, **RW** в режиме записи.
  * **gender** - пол пациента, **RO** в режиме чтения, **RW** в режиме записи.
  * **birthdate** - дата рождения пациента, **RO** в режиме чтения, **RW** в режиме записи.
  * **patient_name** - имя пациента, **RO** в режиме чтения, **RW** в режиме записи.
  * **patient_additional** - дополнительно о пациенте, **RO** в режиме чтения, **RW** в режиме записи.
  * **admincode** - код админа, **RO** в режиме чтения, **RW** в режиме записи.
  * **technician** - техническое поле, **RO** в режиме чтения, **RW** в режиме записи.
  * **equipment** - оборудование, **RO** в режиме чтения, **RW** в режиме записи.
  * **recording_additional**, **RO** в режиме чтения, **RW** в режиме записи.
  * **datarecord_duration** - Продолжительность datarecord выражается в единицах 100 наносекунд, **RO** в режиме чтения, **RW** в режиме записи.
  * **datarecords_in_file** - количество datarecords в файле, **RO**
  * **annotations_in_file** - количество аннотаций в файле, **RO**

  * **signalparam** - массив JSON-объектов с информацией по всем сигналам. Каждый JSON-объект включает следующие ключи, описаные в спецификации к форматам edf/edf+, **RO**:

    * ***label*** - название сигнала, строковое поле
    * ***smp_in_file*** - количество семплов, записанных в файле, числовое поле
    * ***phys_max*** - физический максимум значения сигнала, числовое поле
    * ***phys_min*** - физический минимум значения сигнала, числовое поле
    * ***dig_max*** - логический максимум значения сигнала, числовое поле
    * ***dig_min*** - логический минимум значения сигнала, числовое поле
    * ***smp_in_datarecord*** - количество сигналов в записи, числовое поле
    * ***physdimension*** - еденица измерения (например uV", "BPM", "mA", "Degr.", итд), строковое поле
    * ***prefilter*** - префильтр (например "HP:0.1Hz", "LP:75Hz N:50Hz", итд), строковое поле
    * ***transducer*** - название датчика (например "AgAgCl cup electrodes," , итд), строковое поле
  * **annotations** - синхронное чтение аннотаций, возвращает массив JSON-объектов с информацией по всем аннотациям, **RO**:
    
    * ***onset*** - Время (относительно starttime) в 100 наносекунд (0.0001 сек) события, числовое поле
    * ***duration*** - продолжительность события, строковое поле
    * ***annotation*** - описание события, строковое поле
  
