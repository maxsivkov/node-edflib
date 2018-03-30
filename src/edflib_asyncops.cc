#include <time.h>
#include "edf_wrapper.h"

/// ********** EdfOpenReadWorker

EdfOpenReadWorker::EdfOpenReadWorker(Callback *callback, EdfModule &m) : AsyncWorker(callback)
	, m_path(""), m_result(""), m_error(""), m_module(m) {};
EdfOpenReadWorker::~EdfOpenReadWorker() {};

void EdfOpenReadWorker::Execute() {
	//clock_t sleep = CLOCKS_PER_SEC * 5 + clock();
	//while (sleep > clock());
	int err = edfopen_file_readonly(m_path.c_str(), &m_module.edfInfo(), m_readAnnotations);
	if (err < 0)
	{
		std::stringstream ss;
		ss << "edflib error " << m_module.edfInfo().filetype << " [" << edferror_text(m_module.edfInfo().filetype) << "]";
		m_error = ss.str();
		m_module.m_openMode = EdfModule::NoneMode;
	} else 
	{
		m_module.m_fileOpened = true;
		m_module.m_openMode = EdfModule::ReadMode;
		m_result = "OK";
	}
};

void EdfOpenReadWorker::HandleOKCallback() {
	HandleScope scope;
	Local<Value> argv[] = {
		New<String>(m_error.c_str()).ToLocalChecked(),
		New<String>(m_result.c_str()).ToLocalChecked()
	};

	callback->Call(2, argv);
};

NAN_METHOD(EdfOpenReadWorker::OpenRead) {
	std::stringstream ss;
	v8::Local<v8::Function> _callback;
	//ReadAnnotations readAnnotations;
	Local<v8::Integer> readAnnotations = Nan::New<v8::Integer>(EDFLIB_DO_NOT_READ_ANNOTATIONS);
	std::string error = WRONG_ARGUMENTS_TXT;
	Local<v8::String> path;
	
	//ss << "info length " << info.Length();
	if (Nan::Check(info).ArgumentsCount(3)
		.Argument(0).NotNull().Bind(path)
		.Argument(1).NotNull().Bind(readAnnotations)
		.Argument(2).IsFunction().Bind(_callback).Error(&error)

		&& Utf8String(path).length() > 0
	)
	{
		Callback *callback = new Callback(_callback/*info[1].As<Function>()*/);
		EdfModule* obj = node::ObjectWrap::Unwrap<EdfModule>(info.This());
		EdfOpenReadWorker *worker = new EdfOpenReadWorker(callback, *obj);
		worker->m_path = *Utf8String(path);
		worker->m_readAnnotations = To<int>(readAnnotations).FromMaybe(EDFLIB_DO_NOT_READ_ANNOTATIONS);
		worker->m_result = ss.str();
		AsyncQueueWorker(worker);
	} else
		ThrowSyntaxError(error.c_str());
};


/// ********** EdfOpenWriteWorker

EdfOpenWriteWorker::EdfOpenWriteWorker(Callback *callback, EdfModule &m) : AsyncWorker(callback)
, m_path(""), m_result(""), m_error(""), m_module(m) {};
EdfOpenWriteWorker::~EdfOpenWriteWorker() {};

void EdfOpenWriteWorker::Execute() {
	int err = edfopen_file_writeonly(m_path.c_str(), m_fileType, m_signalsNo);
	if (err < 0)
	{
		std::stringstream ss;
		ss << "edflib error " << err << " [" << edferror_text(err) << "]";
		m_error = ss.str();
		m_module.m_openMode = EdfModule::NoneMode;
	}
	else
	{
		m_module.m_fileOpened = true;
		m_module.m_openMode = EdfModule::WriteMode;
		std::memset(&m_module.edfInfo(), 0, sizeof(m_module.edfInfo()));
		m_module.edfInfo().handle = err;
		m_module.n_currentWriteSignalNo = 0;
		
		m_module.m_write_signals_info.clear();
		for (int i = 0; i < m_signalsNo; i++)
			m_module.m_write_signals_info.insert(std::make_pair(i, EdfModule::signal_info_t()));
		

		m_result = "OK";
	}
};

void EdfOpenWriteWorker::HandleOKCallback() {
	HandleScope scope;
	Local<Value> argv[] = {
		New<String>(m_error.c_str()).ToLocalChecked(),
		New<String>(m_result.c_str()).ToLocalChecked()
	};

	callback->Call(2, argv);
};

NAN_METHOD(EdfOpenWriteWorker::OpenWrite) {
	v8::Local<v8::Function> _callback;
	Local<v8::Integer> fileType = Nan::New<v8::Integer>(EDFLIB_FILETYPE_EDFPLUS), signalsNo = Nan::New<v8::Integer>(0);
	std::string error = WRONG_ARGUMENTS_TXT;
	Local<v8::String> path;

	//ss << "info length " << info.Length();
	if (Nan::Check(info).ArgumentsCount(4)
		.Argument(0).NotNull().Bind(path)
		.Argument(1).NotNull().Bind(fileType)
		.Argument(2).NotNull().Bind(signalsNo)
		.Argument(3).IsFunction().Bind(_callback).Error(&error)

		//&& Utf8String(path).length() > 0 && (To<int>(fileType).FromJust() == EDFLIB_FILETYPE_EDFPLUS || To<int>(fileType).FromJust() == EDFLIB_FILETYPE_BDFPLUS) 
		//&& To<int>(signalsNo).FromJust() > 0
		)
	{
		Callback *callback = new Callback(_callback/*info[1].As<Function>()*/);
		EdfModule* obj = node::ObjectWrap::Unwrap<EdfModule>(info.This());
		EdfOpenWriteWorker *worker = new EdfOpenWriteWorker(callback, *obj);
		worker->m_path = *Utf8String(path);
		worker->m_fileType = To<int>(fileType).FromJust();
		worker->m_signalsNo = To<int>(signalsNo).FromJust();
		AsyncQueueWorker(worker);
	}
	else
		ThrowSyntaxError(error.c_str());
};



/// ********** EdfCloseFileWorker

EdfCloseFileWorker::EdfCloseFileWorker(Callback *callback, EdfModule &m) : AsyncWorker(callback)
, m_module(m), m_error(""){};
EdfCloseFileWorker::~EdfCloseFileWorker() {};

void EdfCloseFileWorker::Execute() {
	int err = edfclose_file(m_module.edfInfo().handle);
	if (err < 0) {
		std::stringstream ss;
		ss << EDF_LIB_ERROR << " " <<err << " [" << edferror_text(err) << "]";
		m_error = ss.str();
	}
	m_module.clear();
};

void EdfCloseFileWorker::HandleOKCallback() {
	HandleScope scope;
	Local<Value> argv[] = {
		New<String>(m_error).ToLocalChecked()
		, New<String>("OK").ToLocalChecked()
	};

	callback->Call(2, argv);
};

NAN_METHOD(EdfCloseFileWorker::CloseFile) {
	v8::Local<v8::Function> _callback;
	EdfModule* obj = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	std::string error = WRONG_ARGUMENTS_TXT;
	if (Nan::Check(info).ArgumentsCount(1)
		.Argument(0).IsFunction().Bind(_callback).Error(&error))
	{
		error = FILE_NOT_OPENED_TXT;
		if (obj->m_fileOpened)
		{
			Callback *callback = new Callback(_callback);

			EdfCloseFileWorker *worker = new EdfCloseFileWorker(callback, *obj);
			AsyncQueueWorker(worker);
		}
		else
			ThrowError(error.c_str());
	} else ThrowError(error.c_str());
};




/// ********** EdfReadPhysicalSamplesWorker

EdfReadPhysicalSamplesWorker::EdfReadPhysicalSamplesWorker(Callback *callback, EdfModule &m, const int &signal, const int &n) : AsyncWorker(callback)
, m_module(m), m_signal(signal), m_number(n){};
EdfReadPhysicalSamplesWorker::~EdfReadPhysicalSamplesWorker() {
	
};

void EdfReadPhysicalSamplesWorker::Execute() {
	m_buf.resize(m_number);
	EdfModule::edf_lock_guard_t lock(m_module.m_edfLock);
	int err = edfread_physical_samples(m_module.edfInfo().handle, m_signal, m_number, m_buf.data());
	if (err >= 0)
		m_number = err;
	else {
		m_buf.clear();
		std::stringstream ss;
		ss << "Edf error occured " << err << " [" << edferror_text(err) << "]";
		m_error = ss.str();
	}
};

void EdfReadPhysicalSamplesWorker::HandleOKCallback() {
	Local<v8::Array> results = New<v8::Array>(m_buf.size());
	for (buf_t::size_type i = 0; i < m_buf.size(); i++)
		Nan::Set(results, i, Nan::New<v8::Number>(m_buf[i]));

	Local<Value> argv[] = {
		New<String>(m_error).ToLocalChecked()
		/*Null()*/
		, results
	};

	callback->Call(2, argv);
};

NAN_METHOD(EdfReadPhysicalSamplesWorker::ReadPhysicalSamples) {
	v8::Local<v8::Function> _callback;
	EdfModule* obj = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	std::string error = WRONG_ARGUMENTS_TXT;
	Local<v8::Number> signal = Nan::New<v8::Number>(-1), n = Nan::New<v8::Number>(-1);
	if (Nan::Check(info).ArgumentsCount(3)
		.Argument(0).NotNull().Bind(signal)
		.Argument(1).NotNull().Bind(n)
		.Argument(2).IsFunction().Bind(_callback).Error(&error)
		&& To<int>(signal).FromMaybe(-1) >= 0 && To<int>(n).FromMaybe(-1) >= 0
		)
	{
		error = FILE_NOT_OPENED_TXT;
		if (obj->m_fileOpened)
		{
			Callback *callback = new Callback(_callback);

			EdfReadPhysicalSamplesWorker * worker = new EdfReadPhysicalSamplesWorker(callback, *obj, To<int>(signal).FromMaybe(0), To<int>(n).FromMaybe(0));
			AsyncQueueWorker(worker);
		}
		else
			ThrowError(error.c_str());
	} else ThrowError(error.c_str());
};




/// ********** EdfReadDigitalSamplesWorker

EdfReadDigitalSamplesWorker::EdfReadDigitalSamplesWorker(Callback *callback, EdfModule &m, const int &signal, const int &n) : AsyncWorker(callback)
, m_module(m), m_signal(signal), m_number(n) {};
EdfReadDigitalSamplesWorker::~EdfReadDigitalSamplesWorker() {

}

void EdfReadDigitalSamplesWorker::Execute() {
	m_buf.resize(m_number);
	EdfModule::edf_lock_guard_t lock(m_module.m_edfLock);
	int err = edfread_digital_samples(m_module.edfInfo().handle, m_signal, m_number, m_buf.data());
	if (err >= 0)
		m_number = err;
	else {
		m_buf.clear();
		std::stringstream ss;
		ss << "Edf error occured " << err << " [" << edferror_text(err) << "]";
		m_error = ss.str();
	}
}

void EdfReadDigitalSamplesWorker::HandleOKCallback() {
	Local<v8::Array> results = New<v8::Array>(m_buf.size());
	for (buf_t::size_type i = 0; i < m_buf.size(); i++)
		Nan::Set(results, i, Nan::New<v8::Number>(m_buf[i]));

	Local<Value> argv[] = {
		New<String>(m_error).ToLocalChecked()
		, results
	};

	callback->Call(2, argv);
}

NAN_METHOD(EdfReadDigitalSamplesWorker::ReadDigitalSamples) {
	v8::Local<v8::Function> _callback;
	EdfModule* obj = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	std::string error = WRONG_ARGUMENTS_TXT;
	Local<v8::Number> signal = Nan::New<v8::Number>(-1), n = Nan::New<v8::Number>(-1);
	if (Nan::Check(info).ArgumentsCount(3)
		.Argument(0).NotNull().Bind(signal)
		.Argument(1).NotNull().Bind(n)
		.Argument(2).IsFunction().Bind(_callback).Error(&error)
		&& To<int>(signal).FromMaybe(-1) >= 0 && To<int>(n).FromMaybe(-1) >= 0
		)
	{
		error = FILE_NOT_OPENED_TXT;
		if (obj->m_fileOpened)
		{
			Callback *callback = new Callback(_callback);

			EdfReadDigitalSamplesWorker * worker = new EdfReadDigitalSamplesWorker(callback, *obj, To<int>(signal).FromMaybe(0), To<int>(n).FromMaybe(0));
			AsyncQueueWorker(worker);
		}
		else
			ThrowError(error.c_str());
	}
	else ThrowError(error.c_str());
}

/// ********** EdfReadAnnotationsWorker

EdfReadAnnotationsWorker::EdfReadAnnotationsWorker(Callback *callback, EdfModule &m, const int &n) : AsyncWorker(callback)
, m_module(m), m_number(n){};
EdfReadAnnotationsWorker::~EdfReadAnnotationsWorker() {

}

void EdfReadAnnotationsWorker::Execute() {
	if (!m_module.m_fileOpened || !m_module.isReadMode())
	{
		m_error = FILE_NOT_OPENED_TXT;
		return;
	}
	int n = m_number > 0 ? m_number : m_module.edfInfo().annotations_in_file;
	if (n > 0)
	{
		m_items.resize(n);
		for (int i = 0; i < m_items.size(); i++)
		{
			std::memset(&m_items.at(i), 0, sizeof(m_items.at(i)));
			int err = edf_get_annotation(m_module.edfInfo().handle, i, &m_items.at(i));
			if (err < 0) {
				std::stringstream ss;
				ss << "Error obtaining annotation #" << i << " Edf error " << err << " [" << edferror_text(err) << "]";;
				m_error = ss.str();
				m_items.clear();
				break;
			}
		}
	}
	else m_items.clear();
}

void EdfReadAnnotationsWorker::HandleOKCallback() {
	Local<v8::Array> results = New<v8::Array>(m_items.size());
	for (int i = 0; i < m_items.size(); i++)
	{
		Local<v8::Object> itm = Nan::New<v8::Object>();
		itm->Set(Nan::New("onset").ToLocalChecked(), Nan::New(static_cast<double>(m_items.at(i).onset)));
		itm->Set(Nan::New("duration").ToLocalChecked(), Nan::New(m_items.at(i).duration).ToLocalChecked());
		itm->Set(Nan::New("annotation").ToLocalChecked(), Nan::New(m_items.at(i).annotation).ToLocalChecked());
		Nan::Set(results, i, itm);
	}

	Local<Value> argv[] = {
		New<String>(m_error).ToLocalChecked()
		, results
	};

	callback->Call(2, argv);
}

NAN_METHOD(EdfReadAnnotationsWorker::ReadAnnotations) {
	v8::Local<v8::Function> _callback;
	EdfModule* obj = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	std::string error = WRONG_ARGUMENTS_TXT;
	
	Local<v8::Number> n = Nan::New<v8::Number>(-1);
	if (Nan::Check(info).ArgumentsCount(2)
		.Argument(0).NotNull().Bind(n)
		.Argument(1).IsFunction().Bind(_callback).Error(&error)
		&& To<int>(n).FromMaybe(-1) >= 0
		)
	{
		error = FILE_NOT_OPENED_TXT;
		if (obj->m_fileOpened && obj->isReadMode())
		{
			Callback *callback = new Callback(_callback);

			EdfReadAnnotationsWorker * worker = new EdfReadAnnotationsWorker(callback, *obj, To<int>(n).FromMaybe(-1));
			AsyncQueueWorker(worker);
		}
		else
			ThrowError(error.c_str());
	}
	else ThrowError(error.c_str());
}


/// ********** WritePhysicalSamplesWorker

WriteSamplesWorker::WriteSamplesWorker(Callback *callback, EdfModule &m) : AsyncWorker(callback)
, m_module(m){};
WriteSamplesWorker::~WriteSamplesWorker() {
}
bool WriteSamplesWorker::basicChecks(EdfModule* pmodule, Nan::NAN_METHOD_ARGS_TYPE info, Local<v8::Array> &buf, Local<Function> &callback, std::string &error
	, std::function<bool(Local<v8::Array>&)> additional_check)
{
	bool result = Nan::Check(info).ArgumentsCount(2)
		.Argument(0).NotNull().Bind(buf)
		.Argument(1).IsFunction().Bind(callback).Error(&error) && additional_check(buf);
	if (!result)
		ThrowError(WRONG_ARGUMENTS_TXT);
	else if (!pmodule->m_fileOpened || !pmodule->isWriteMode())
	{
		result = false;
		ThrowError(FILE_NOT_OPENED_TXT);
	}
	return result;
}

void WriteSamplesWorker::Execute() {
	m_error = "";
	if (!m_module.m_fileOpened || !m_module.isWriteMode() )
	{
		m_error = FILE_NOT_OPENED_TXT;
		return;
	}
	int err = 0;
	if (m_writeHandler.get()) 
	{
		m_writeHandler->write();
		m_error = m_writeHandler->m_error;
		m_result = m_writeHandler->m_result;
	}
}

void WriteSamplesWorker::HandleOKCallback() {
	Local<Value> argv[] = {
		New<String>(m_error).ToLocalChecked()
		, New<String>(m_result).ToLocalChecked()
	};

	callback->Call(2, argv);
}

NAN_METHOD(WriteSamplesWorker::WritePhysicalSamples) {
	WriteBufSamples<write_phys_samples_t>(info);
}

NAN_METHOD(WriteSamplesWorker::WriteDigitalShortSamples)
{
	WriteBufSamples<write_digital_short_samples_t>(info);
}

NAN_METHOD(WriteSamplesWorker::WriteDigitalSamples)
{
	WriteBufSamples<write_digital_samples_t>(info);
}


NAN_METHOD(WriteSamplesWorker::BlockWritePhysicalSamples) {
	WriteBlockSamples<block_write_phys_samples_t>(info);
}

NAN_METHOD(WriteSamplesWorker::BlockWriteDigitalShortSamples) {
	WriteBlockSamples<block_write_digital_short_samples_t>(info);
}

NAN_METHOD(WriteSamplesWorker::BlockWriteDigitalSamples) {
	WriteBlockSamples<block_write_digital_samples_t>(info);
}

NAN_METHOD(WriteSamplesWorker::BlockWriteDigital3ByteSamples) {
	WriteBlockSamples<block_write_digital_3byte_samples_t>(info);
}
