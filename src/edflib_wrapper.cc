
#include <time.h>
#include <sstream>
#include "edf_wrapper.h"

Persistent<Function> EdfModule::constructor;
Persistent<Function> EdfModule::m_loggerCb;

const char *EDF_WRAPPER_VERSION = "0.01";
int gc = 0;
int next_gc() { return gc++; }
#define DECLARE_EDF_PROP_FULL(Name, PropAttr) Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New(#Name).ToLocalChecked(), prp_header_get_##Name, prp_header_set_##Name, v8::Local<v8::Value>(), v8::DEFAULT, PropAttr)
#define DECLARE_EDF_PROP_RO(Name) DECLARE_EDF_PROP_FULL(Name, v8::PropertyAttribute::ReadOnly)
#define DECLARE_EDF_PROP(Name) DECLARE_EDF_PROP_FULL(Name, v8::PropertyAttribute::None)

#define DECLARE_EDF_DEFINE(Name, WrapperName) Nan::SetAccessor(target, Nan::New(WrapperName).ToLocalChecked(), const_getter##Name, 0, v8::Local<v8::Value>(), v8::DEFAULT, v8::PropertyAttribute::ReadOnly)
//Set(target, Nan::New(#Name).ToLocalChecked(), GetFunction(Nan::New<FunctionTemplate>(EdfModule::const_getter##Name)).ToLocalChecked())

std::string trim_str(const char*s) { return trim(std::string(s)); }

//
NAN_MODULE_INIT(EdfModule::Init) {
	Set(
		target,
		Nan::New("GetVersion").ToLocalChecked(),
		GetFunction(Nan::New<FunctionTemplate>(EdfModule::GetVersion)).ToLocalChecked()
	);
	Set(
		target,
		Nan::New("SetLogger").ToLocalChecked(),
		GetFunction(Nan::New<FunctionTemplate>(EdfModule::SetLogger)).ToLocalChecked()
	);
	/*Set(
		target,
		New("OpenRead").ToLocalChecked(),
		GetFunction(New<FunctionTemplate>(ModuleWorker::OpenRead)).ToLocalChecked()
	);*/
	// Prepare constructor template
	v8::Local<v8::FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
	tpl->SetClassName(Nan::New<v8::String>("Edf").ToLocalChecked());
	tpl->InstanceTemplate()->SetInternalFieldCount(1);
	//NODE_SET_PROTOTYPE_METHOD(tpl, "OpenRead", EdfOpenReadWorker::OpenRead);
	//Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("Header").ToLocalChecked(), prp_GetHeader);
	//Nan::SetNamedPropertyHandler(tpl->InstanceTemplate(), Nan::New("Header").ToLocalChecked(), prp_GetHeader, )
	SetPrototypeMethod(tpl, "OpenRead", EdfOpenReadWorker::OpenRead);
	SetPrototypeMethod(tpl, "OpenWrite", EdfOpenWriteWorker::OpenWrite);
	SetPrototypeMethod(tpl, "Close", EdfCloseFileWorker::CloseFile);
	SetPrototypeMethod(tpl, "ReadPhysicalSamples", EdfReadPhysicalSamplesWorker::ReadPhysicalSamples);
	SetPrototypeMethod(tpl, "ReadDigitalSamples", EdfReadDigitalSamplesWorker::ReadDigitalSamples);
	SetPrototypeMethod(tpl, "ReadAnnotations", EdfReadAnnotationsWorker::ReadAnnotations);
	SetPrototypeMethod(tpl, "SeekSync", EdfModule::EdfSeek);
	SetPrototypeMethod(tpl, "TellSync", EdfModule::EdfTell);
	SetPrototypeMethod(tpl, "RewindSync", EdfModule::EdfRewind);
	SetPrototypeMethod(tpl, "SetSampleFrequencySync", EdfModule::SetSampleFrequency);
	SetPrototypeMethod(tpl, "SetPhysicalParametersSync", EdfModule::SetPhysicalParameters);
	SetPrototypeMethod(tpl, "SetDigitalParametersSync", EdfModule::SetDigitalParameters);
	SetPrototypeMethod(tpl, "SetLabelSync", EdfModule::SetLabel);
	SetPrototypeMethod(tpl, "SetPrefilterSync", EdfModule::SetPrefilter);
	SetPrototypeMethod(tpl, "SetTransducerSync", EdfModule::SetTransducer);
	SetPrototypeMethod(tpl, "SetPhysicalDimensionSync", EdfModule::SetPhysicalDimension);

	SetPrototypeMethod(tpl, "WritePhysicalSamples", WriteSamplesWorker::WritePhysicalSamples);
	SetPrototypeMethod(tpl, "BlockWritePhysicalSamples", WriteSamplesWorker::BlockWritePhysicalSamples);
	SetPrototypeMethod(tpl, "WriteDigitalShortSamples", WriteSamplesWorker::WriteDigitalShortSamples);
	SetPrototypeMethod(tpl, "WriteDigitalSamples", WriteSamplesWorker::WriteDigitalSamples);
	SetPrototypeMethod(tpl, "BlockWriteDigitalShortSamples", WriteSamplesWorker::BlockWriteDigitalShortSamples);
	SetPrototypeMethod(tpl, "BlockWriteDigitalSamples", WriteSamplesWorker::BlockWriteDigitalSamples);
	SetPrototypeMethod(tpl, "BlockWriteDigital3ByteSamples", WriteSamplesWorker::BlockWriteDigital3ByteSamples);
	SetPrototypeMethod(tpl, "WriteAnnotationSync", EdfModule::WriteAnnotationUtf8);
	SetPrototypeMethod(tpl, "WriteAnnotationLatin1Sync", EdfModule::WriteAnnotationLatin1);
	SetPrototypeMethod(tpl, "SetNumberOfAnnotationSignals", EdfModule::SetNumberOfAnnotationSignals);
	Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("signalparam").ToLocalChecked(), signalparam_getter);
	Nan::SetAccessor(tpl->InstanceTemplate(), Nan::New("annotations").ToLocalChecked(), annotations_getter);
	
	DECLARE_EDF_DEFINE(EDFLIB_DO_NOT_READ_ANNOTATIONS, "DO_NOT_READ_ANNOTATIONS");
	DECLARE_EDF_DEFINE(EDFLIB_READ_ANNOTATIONS, "READ_ANNOTATIONS");
	DECLARE_EDF_DEFINE(EDFLIB_READ_ALL_ANNOTATIONS, "READ_ALL_ANNOTATIONS");
	DECLARE_EDF_DEFINE(EDFSEEK_SET, "SEEK_SET");
	DECLARE_EDF_DEFINE(EDFSEEK_CUR, "SEEK_CUR");
	DECLARE_EDF_DEFINE(EDFSEEK_END, "SEEK_END");

	DECLARE_EDF_DEFINE(EDFLIB_FILETYPE_EDF, "FILETYPE_EDF");
	DECLARE_EDF_DEFINE(EDFLIB_FILETYPE_EDFPLUS, "FILETYPE_EDFPLUS");
	DECLARE_EDF_DEFINE(EDFLIB_FILETYPE_BDF, "FILETYPE_BDF");
	DECLARE_EDF_DEFINE(EDFLIB_FILETYPE_BDFPLUS, "FILETYPE_BDFPLUS");


	//SetPrototypeMethod(tpl, "Header", Header);
	
	DECLARE_EDF_PROP_RO(handle);
	DECLARE_EDF_PROP_RO(filetype);
	DECLARE_EDF_PROP_RO(edfsignals);
	DECLARE_EDF_PROP_RO(file_duration);
	DECLARE_EDF_PROP(startdate_day);
	DECLARE_EDF_PROP(startdate_month);
	DECLARE_EDF_PROP(startdate_year);
	DECLARE_EDF_PROP(starttime_subsecond);
	DECLARE_EDF_PROP(starttime_second);
	DECLARE_EDF_PROP(starttime_minute);
	DECLARE_EDF_PROP(starttime_hour);

	DECLARE_EDF_PROP(patient);
	DECLARE_EDF_PROP(recording);
	DECLARE_EDF_PROP(patientcode);
	DECLARE_EDF_PROP(gender);
	DECLARE_EDF_PROP(birthdate);
	DECLARE_EDF_PROP(patient_name);
	DECLARE_EDF_PROP(patient_additional);
	DECLARE_EDF_PROP(admincode);
	DECLARE_EDF_PROP(technician);
	DECLARE_EDF_PROP(equipment);
	DECLARE_EDF_PROP(recording_additional);
	DECLARE_EDF_PROP(datarecord_duration);
	DECLARE_EDF_PROP(datarecords_in_file);
	DECLARE_EDF_PROP(annotations_in_file);
	//DECLARE_EDF_PROP();
	//DECLARE_EDF_PROP();

	
	SetPrototypeMethod(tpl, "GetVersion", GetVersion);

	constructor.Reset(tpl->GetFunction());
	Set(target, Nan::New("Edf").ToLocalChecked(), tpl->GetFunction());
	
}

void EdfModule::log(const std::ostream &oss)
{
	return;
	if (m_loggerCb.IsEmpty()) return;
	std::string line = "";
	try {
		const std::ostringstream& s = dynamic_cast<const std::ostringstream&>(oss);
		line = s.str();
	}
	catch (std::exception &ex) { line = ex.what(); }
	if (line.empty()) return;
	line = "node-edflib> " + line;
	Local<v8::Function > callback_handle = Local<v8::Function>::Cast(Nan::New(m_loggerCb));
	Local<Value> argv[] = {
		//Nan::New<String>("Call persistent").ToLocalChecked()
		Nan::New<String>(line).ToLocalChecked()
	};
	//callback_handle->Call(GetCurrentContext()->Global(), 1, argv);
	Nan::MakeCallback(GetCurrentContext()->Global(), callback_handle, 1, argv);
}

EdfModule::EdfModule()
{
	clear();
}

void EdfModule::clear()
{
	n_currentWriteSignalNo = 0;
	m_openMode = NoneMode;
	m_write_signals_info.clear();
	std::memset(&m_edfInfo, 0, sizeof(m_edfInfo));
	m_fileOpened = false;
	m_startdaySetters = StartdayFields::ST_INITIAL;

}

EdfModule::~EdfModule() 
{
	if (m_fileOpened)
		edfclose_file(edfInfo().handle);
	m_fileOpened = false;
}
/*
NAN_GETTER(EdfModule::prp_GetHeader) {
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	v8::Isolate *isolate = info.GetIsolate();
	v8::String::Utf8Value s(property);
	std::string property_str(*s);

	if (property_str == "filetype") info.GetReturnValue().Set(v8::Number::New(isolate, self->m_edfInfo.filetype));
}
*/
NAN_METHOD(EdfModule::GetVersion) {
	std::stringstream ss;
	std::string param0;

	if (Nan::Check(info).ArgumentsCount(0))
		ss << edflib_version();
	else if (Nan::Check(info).ArgumentsCount(1)
		.Argument(0).Bind(param0) )
		ss << EDF_WRAPPER_VERSION;
	info.GetReturnValue().Set(Nan::New(ss.str()).ToLocalChecked());
}

NAN_METHOD(EdfModule::SetLogger) {
	//HandleScope scope;
	m_loggerCb.Reset(info[0].As<Function>());
	
	Local<v8::Function > callback_handle = Local<v8::Function>::Cast(Nan::New(m_loggerCb));
	Local<Value> argv[] = {
		Nan::New<String>("Test logging string m_loggerCb").ToLocalChecked()
	};
	//GetCurrentContext()->Global()
	Nan::MakeCallback( GetCurrentContext()->Global()/*info.This()*/, callback_handle, 1, argv);
	
}


NAN_METHOD(EdfModule::EdfSeek) {
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	Local<v8::Integer> signal = Nan::New<v8::Integer>(-1);
	Local<v8::Number> offset = Nan::New<v8::Integer>(0);
	Local<v8::Integer> whence = Nan::New<v8::Integer>(-1);
	int64_t result(0);
	std::string error = FILE_NOT_OPENED_TXT;
	if (self->m_fileOpened)
	{
		error = WRONG_ARGUMENTS_TXT;
		if (Nan::Check(info).ArgumentsCount(3)
			.Argument(0).NotNull().Bind(signal)
			.Argument(1).NotNull().Bind(offset)
			.Argument(2).NotNull().Bind(whence)
			&& To<int>(signal).FromMaybe(-1) >= 0 && To<int>(whence).FromMaybe(-1) >= 0
			)
		{
			result = edfseek(self->edfInfo().handle, To<int>(signal).FromJust(), To<int64_t>(offset).FromJust(), To<int>(whence).FromJust());
			if (result < 0) {
				std::stringstream ss;
				ss << EDF_LIB_ERROR << " " << result;
				ThrowError(ss.str().c_str());
			}
		}
		else ThrowError(error.c_str());
	} else ThrowError(error.c_str());
	info.GetReturnValue().Set(Nan::New<v8::Number>(result));
}

NAN_METHOD(EdfModule::EdfTell) {

	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	Local<v8::Integer> signal = Nan::New<v8::Integer>(-1);
	std::string error = FILE_NOT_OPENED_TXT;
	int64_t result = 0;
	if (self->m_fileOpened)
	{
		error = WRONG_ARGUMENTS_TXT;
		if (Nan::Check(info).ArgumentsCount(1)
			.Argument(0).NotNull().Bind(signal)
			&& To<int>(signal).FromMaybe(-1) >= 0
			)
		{
			result = edftell(self->edfInfo().handle, To<int>(signal).FromJust());
		}
		else ThrowError(error.c_str());
	}
	else ThrowError(error.c_str());
	
	info.GetReturnValue().Set(Nan::New<v8::Number>(result));
}

NAN_METHOD(EdfModule::EdfRewind) {

	bool result = false;
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	Local<v8::Integer> signal = Nan::New<v8::Integer>(-1);
	std::string error = FILE_NOT_OPENED_TXT;
	if (self->m_fileOpened)
	{
		error = WRONG_ARGUMENTS_TXT;
		if (Nan::Check(info).ArgumentsCount(1)
			.Argument(0).NotNull().Bind(signal)
			&& To<int>(signal).FromMaybe(-1) >= 0
			)
		{
			int err = edfrewind(self->edfInfo().handle, To<int>(signal).FromJust());
			if (err < 0) {
				std::stringstream ss;
				ss << EDF_LIB_ERROR << " " << err << " [" << edferror_text(err) << "]";
				ThrowError(ss.str().c_str());
			} else result = true;
		}
		else ThrowError(error.c_str());
	}
	else ThrowError(error.c_str());

	info.GetReturnValue().Set(Nan::New<v8::Boolean>(result));
}

NAN_METHOD(EdfModule::SetSampleFrequency) {

	bool result = false;
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	Local<v8::Integer> signal = Nan::New<v8::Integer>(-1), freq = Nan::New<v8::Integer>(0);
	std::string error = FILE_NOT_OPENED_TXT;
	if (self->m_fileOpened && self->m_openMode == OpenMode::WriteMode)
	{
		error = WRONG_ARGUMENTS_TXT;
		if (Nan::Check(info).ArgumentsCount(2)
			.Argument(0).NotNull().Bind(signal)
			.Argument(1).NotNull().Bind(freq)
			&& To<int>(signal).FromMaybe(-1) >= 0 && To<int>(freq).FromMaybe(-1) > 0
			)
		{
			int err = edf_set_samplefrequency(self->edfInfo().handle, To<int>(signal).FromJust(), To<int>(freq).FromJust());
			if (err < 0) {
				std::stringstream ss;
				ss << EDF_LIB_ERROR << " " << err << " [" << edferror_text(err) << "]";
				ThrowError(ss.str().c_str());
			}
			else {
				try {
					signal_info_t &info = self->m_write_signals_info.at(To<int>(signal).FromJust());
					info.m_samplefrequency = To<int>(freq).FromJust();
					result = true;
				}
				catch (std::logic_error &) 
				{
					ThrowError("Signal not found");
				}
			}
		}
		else ThrowError(error.c_str());
	}
	else ThrowError(error.c_str());

	info.GetReturnValue().Set(Nan::New<v8::Boolean>(result));
}

Nan::NAN_METHOD_RETURN_TYPE EdfModule::WriteAnnotation(Nan::NAN_METHOD_ARGS_TYPE info, std::function<int(int, int64_t, int64_t, const char *)>edf_write_annotation) {

	bool result = false;
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	Local<v8::Number> onset = Nan::New<v8::Number>(-1), duration = Nan::New<v8::Number>(-1);
	Local<v8::String> description = Local<v8::String>();
	std::string error = FILE_NOT_OPENED_TXT;
	if (self->m_fileOpened && self->m_openMode == OpenMode::WriteMode)
	{
		error = WRONG_ARGUMENTS_TXT;
		if (Nan::Check(info).ArgumentsCount(3)
			.Argument(0).NotNull().Bind(onset)
			.Argument(1).NotNull().Bind(duration)
			.Argument(2).NotNull().Bind(description)
			&& To<int64_t>(onset).FromMaybe(-1) >= 0 && To<int64_t>(duration).FromMaybe(-1) >= 0
			)
		{
			int err = edf_write_annotation(self->edfInfo().handle, To<int64_t>(onset).FromJust(), To<int64_t>(duration).FromJust(), *Utf8String(description));
			if (err < 0) {
				std::stringstream ss;
				ss << EDF_LIB_ERROR << " " << err << " [" << edferror_text(err) << "]";
				ThrowError(ss.str().c_str());
			}
			else {
				try {
					//signal_info_t &info = self->m_write_signals_info.at(To<int>(signal).FromJust());
					//info.m_samplefrequency = To<int>(freq).FromJust();
					result = true;
				}
				catch (std::logic_error &)
				{
					ThrowError("Signal not found");
				}
			}
		}
		else ThrowError(error.c_str());
	}
	else ThrowError(error.c_str());

	info.GetReturnValue().Set(Nan::New<v8::Boolean>(result));
}

NAN_METHOD(EdfModule::WriteAnnotationUtf8) {
	WriteAnnotation(info, edfwrite_annotation_utf8);
}
NAN_METHOD(EdfModule::WriteAnnotationLatin1) {
	WriteAnnotation(info, edfwrite_annotation_latin1);
}


NAN_METHOD(EdfModule::SetNumberOfAnnotationSignals) {
	bool result = false;
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	Local<v8::Integer> n = Nan::New<v8::Integer>(1);
	std::string error = FILE_NOT_OPENED_TXT;
	if (self->m_fileOpened && self->m_openMode == OpenMode::WriteMode)
	{
		error = WRONG_ARGUMENTS_TXT;
		if (Nan::Check(info).ArgumentsCount(1)
			.Argument(0).NotNull().Bind(n)
			&& To<int>(n).FromMaybe(1) > 0 && To<int>(n).FromMaybe(1) <= 64
			)
		{
			int err = edf_set_number_of_annotation_signals(self->edfInfo().handle, To<int>(n).FromJust());
			if (err < 0) {
				std::stringstream ss;
				ss << EDF_LIB_ERROR << " " << err << " [" << edferror_text(err) << "]";
				ThrowError(ss.str().c_str());
			}
		}
		else ThrowError(error.c_str());
	}
	else ThrowError(error.c_str());

	info.GetReturnValue().Set(Nan::New<v8::Boolean>(result));
}


Nan::NAN_METHOD_RETURN_TYPE EdfModule::SetSignalStringProp(Nan::NAN_METHOD_ARGS_TYPE info, std::function<int(int, int, const char *)>edf_set_func) {
	bool result = false;
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	Local<v8::Integer> signal = Nan::New<v8::Integer>(-1);
	Local<v8::String> parameter = Local<v8::String>();
	std::string error = FILE_NOT_OPENED_TXT;
	if (self->m_fileOpened && self->m_openMode == OpenMode::WriteMode)
	{
		error = WRONG_ARGUMENTS_TXT;
		if (Nan::Check(info).ArgumentsCount(2)
			.Argument(0).NotNull().Bind(signal)
			.Argument(1).NotNull().Bind(parameter)
			&& To<int64_t>(signal).FromMaybe(-1) >= 0
			)
		{
			int err = edf_set_func(self->edfInfo().handle, To<int>(signal).FromJust(), *Utf8String(parameter));
			if (err < 0) {
				std::stringstream ss;
				ss << EDF_LIB_ERROR << " " << err << " [" << edferror_text(err) << "]";
				ThrowError(ss.str().c_str());
			}
		}
		else ThrowError(error.c_str());
	}
	else ThrowError(error.c_str());

	info.GetReturnValue().Set(Nan::New<v8::Boolean>(result));
}

NAN_METHOD(EdfModule::SetLabel) {
	SetSignalStringProp(info, edf_set_label);
}
NAN_METHOD(EdfModule::SetPrefilter) {
	SetSignalStringProp(info, edf_set_prefilter);
}
NAN_METHOD(EdfModule::SetTransducer) {
	SetSignalStringProp(info, edf_set_transducer);
}
NAN_METHOD(EdfModule::SetPhysicalDimension) {
	SetSignalStringProp(info, edf_set_physical_dimension);
}


NAN_METHOD(EdfModule::SetPhysicalParameters) {

	bool result = false;
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	Local<v8::Integer> signal = Nan::New<v8::Integer>(-1);
	Local<v8::Number>  pmin = Nan::New<v8::Number>(0), pmax = Nan::New<v8::Number>(0);
	std::string error = FILE_NOT_OPENED_TXT;
	if (self->m_fileOpened && self->m_openMode == OpenMode::WriteMode)
	{
		error = WRONG_ARGUMENTS_TXT;
		if (Nan::Check(info).ArgumentsCount(3)
			.Argument(0).NotNull().Bind(signal)
			.Argument(1).NotNull().Bind(pmin)
			.Argument(2).NotNull().Bind(pmax)
			&& To<int>(signal).FromMaybe(-1) >= 0
			)
		{
			int err1 = edf_set_physical_minimum(self->edfInfo().handle, To<int>(signal).FromJust(), To<double>(pmin).FromJust());
			int err2 = edf_set_physical_maximum(self->edfInfo().handle, To<int>(signal).FromJust(), To<double>(pmax).FromJust());
			if (err1 < 0 || err2 < 0) {
				std::stringstream ss;
				ss << EDF_LIB_ERROR << " min: " << err1 << " [" << edferror_text(err1) << "], max "
					<< err2 << " [" << edferror_text(err2) << "]";
				ThrowError(ss.str().c_str());
			}
			else {
				try {
					signal_info_t &info = self->m_write_signals_info.at(To<int>(signal).FromJust());
					info.m_phys_min = To<double>(pmin).FromJust();
					info.m_phys_max = To<double>(pmax).FromJust();
					result = true;
				}
				catch (std::logic_error &)
				{
					ThrowError("Signal not found");
				}
			}
		}
		else ThrowError(error.c_str());
	}
	else ThrowError(error.c_str());

	info.GetReturnValue().Set(Nan::New<v8::Boolean>(result));
}

NAN_METHOD(EdfModule::SetDigitalParameters) {

	bool result = false;
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	Local<v8::Integer> signal = Nan::New<v8::Integer>(-1), pmin = Nan::New<v8::Integer>(0), pmax = Nan::New<v8::Integer>(0);
	std::string error = FILE_NOT_OPENED_TXT;
	if (self->m_fileOpened && self->m_openMode == OpenMode::WriteMode)
	{
		error = WRONG_ARGUMENTS_TXT;
		if (Nan::Check(info).ArgumentsCount(3)
			.Argument(0).NotNull().Bind(signal)
			.Argument(1).NotNull().Bind(pmin)
			.Argument(2).NotNull().Bind(pmax)
			&& To<int>(signal).FromMaybe(-1) >= 0
			)
		{
			int err1 = edf_set_digital_minimum(self->edfInfo().handle, To<int>(signal).FromJust(), To<int>(pmin).FromJust());
			int err2 = edf_set_digital_maximum(self->edfInfo().handle, To<int>(signal).FromJust(), To<int>(pmax).FromJust());
			if (err1 < 0 || err2 < 0) {
				std::stringstream ss;
				ss << EDF_LIB_ERROR << " min: " << err1 << " [" << edferror_text(err1) << "], max "
					<< err2 << " [" << edferror_text(err2) << "]";
				ThrowError(ss.str().c_str());
			}
			else {
				try {
					signal_info_t &info = self->m_write_signals_info.at(To<int>(signal).FromJust());
					info.m_digital_min = To<int>(pmin).FromJust();
					info.m_digital_max = To<int>(pmax).FromJust();
					result = true;
				}
				catch (std::logic_error &)
				{
					ThrowError("Signal not found");
				}
			}
		}
		else ThrowError(error.c_str());
	}
	else ThrowError(error.c_str());

	info.GetReturnValue().Set(Nan::New<v8::Boolean>(result));
}


/*
NAN_METHOD(EdfModule::prp_Header) {
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	v8::Isolate *isolate = info.GetIsolate();
	v8::Local<v8::Object> result = v8::Object::New(isolate);
	result->Set(String::NewFromUtf8(isolate, "filetype"), v8::Number::New(isolate, self->m_edfInfo.filetype));
	result->Set(String::NewFromUtf8(isolate, "edfsignals"), v8::Number::New(isolate, self->m_edfInfo.edfsignals));
	//result->Set(v8::String::NewSymbol("filetype"), v8::String::NewSymbol(self->m_edfInfo.filetype));
	info.GetReturnValue().Set(result);
}
NAN_SETTER(EdfModule::prp_SetHeader)
{
	double d = To<double>(value).FromMaybe(0);
	//v8::Integer::
	//value->From
}
*/
#define EDF_HEADER_SET_ERROR_PROP_IMPL(Name) \
	NAN_SETTER(EdfModule::prp_header_set_##Name) {\
		ThrowError(NOT_IMPLEMENTED_TXT); \
	}


#define EDF_HEADER_GET_INT_PROP_IMPL(Name, NodeType, CType) \
	NAN_GETTER(EdfModule::prp_header_get_##Name) { \
		EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This()); \
		Local<NodeType> result = Nan::New(static_cast<CType>(self->edfInfo().Name)); \
		info.GetReturnValue().Set(result); }
#define EDF_HEADER_SET_INT_PROP_IMPL(Name, NodeType, CType, EdfLibSetter) \
	NAN_SETTER(EdfModule::prp_header_set_##Name) {\
		EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This()); \
		if (self->isReadMode()) ThrowError(READ_ONLY_TXT); else {\
			int res = EdfLibSetter(self->edfInfo().handle, To<CType>(value).FromMaybe(0));\
			if (res){ std::stringstream ss; ss << EDF_LIB_ERROR << " " << res; ThrowError(ss.str().c_str()); } \
			else self->edfInfo().Name = To<CType>(value).FromMaybe(0); } \
		info.GetReturnValue().Set(info.This()); }

#define EDF_HEADER_SET_STARTDATE_INT_PROP_IMPL(Name, CType) \
	NAN_SETTER(EdfModule::prp_header_set_##Name) {\
		EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This()); \
		if (self->isReadMode()) ThrowError(READ_ONLY_TXT); else { \
			self->m_startdaySetters |= StartdayFields::ST_##Name; self->edfInfo().Name = To<CType>(value).FromMaybe(0); \
			if ((self->m_startdaySetters & StartdayFields::ST_MASK) == StartdayFields::ST_MASK) {\
				int res = edf_set_startdatetime(self->edfInfo().handle, self->edfInfo().startdate_year, self->edfInfo().startdate_month, self->edfInfo().startdate_day \
						, self->edfInfo().starttime_hour, self->edfInfo().starttime_minute, self->edfInfo().starttime_second);\
				if (res){ std::stringstream ss; ss << EDF_LIB_ERROR << " " << res; ThrowError(ss.str().c_str()); } \
			} }\
		info.GetReturnValue().Set(info.This()); }



#define EDF_HEADER_GET_STRING_PROP_IMPL(Name, NodeType) \
	NAN_GETTER(EdfModule::prp_header_get_##Name) { \
		EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This()); \
		MaybeLocal<NodeType> result = Nan::New(self->edfInfo().Name); \
		info.GetReturnValue().Set(result.ToLocalChecked()); }

#define EDF_HEADER_SET_STRING_PROP_IMPL(Name, NodeType, EdfLibSetter) \
	NAN_SETTER(EdfModule::prp_header_set_##Name) {\
		EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This()); \
		if (self->isReadMode()) ThrowError(READ_ONLY_TXT); else { \
			int res = EdfLibSetter(self->edfInfo().handle, *Utf8String(value)); \
			if (res) { std::stringstream ss; ss << EDF_LIB_ERROR << " " << res; ThrowError(ss.str().c_str()); } \
			else std::strncpy(self->edfInfo().Name, *Utf8String(value), sizeof(self->edfInfo().Name)); }\
		self->edfInfo().Name[sizeof(self->edfInfo().Name) - 1] = '\0'; \
		info.GetReturnValue().Set(info.This()); }

#define EDFLIB_IMPL_DEFINE_GETTER(Name) \
	NAN_GETTER(EdfModule::const_getter##Name) {info.GetReturnValue().Set(Nan::New(static_cast<int>(Name)));}

EDF_HEADER_GET_INT_PROP_IMPL(handle, v8::Integer, int)				EDF_HEADER_SET_ERROR_PROP_IMPL(handle)

EDF_HEADER_GET_INT_PROP_IMPL(filetype, v8::Integer, int)			EDF_HEADER_SET_ERROR_PROP_IMPL(filetype)

EDF_HEADER_GET_INT_PROP_IMPL(edfsignals, v8::Integer, int)			EDF_HEADER_SET_ERROR_PROP_IMPL(edfsignals)

EDF_HEADER_GET_INT_PROP_IMPL(file_duration, v8::Number, double)		EDF_HEADER_SET_ERROR_PROP_IMPL(file_duration)

EDF_HEADER_GET_INT_PROP_IMPL(startdate_day, v8::Integer, int)		EDF_HEADER_SET_STARTDATE_INT_PROP_IMPL(startdate_day, int)
EDF_HEADER_GET_INT_PROP_IMPL(startdate_month, v8::Integer, int)		EDF_HEADER_SET_STARTDATE_INT_PROP_IMPL(startdate_month, int)
EDF_HEADER_GET_INT_PROP_IMPL(startdate_year, v8::Integer, int)		EDF_HEADER_SET_STARTDATE_INT_PROP_IMPL(startdate_year, int)
EDF_HEADER_GET_INT_PROP_IMPL(starttime_subsecond, v8::Integer, int)		EDF_HEADER_SET_STARTDATE_INT_PROP_IMPL(starttime_subsecond, int)
EDF_HEADER_GET_INT_PROP_IMPL(starttime_second, v8::Integer, int)		EDF_HEADER_SET_STARTDATE_INT_PROP_IMPL(starttime_second, int)
EDF_HEADER_GET_INT_PROP_IMPL(starttime_minute, v8::Integer, int)		EDF_HEADER_SET_STARTDATE_INT_PROP_IMPL(starttime_minute, int)
EDF_HEADER_GET_INT_PROP_IMPL(starttime_hour, v8::Integer, int)		EDF_HEADER_SET_STARTDATE_INT_PROP_IMPL(starttime_hour, int)

EDF_HEADER_GET_STRING_PROP_IMPL(patient, v8::String)				EDF_HEADER_SET_ERROR_PROP_IMPL(patient)
EDF_HEADER_GET_STRING_PROP_IMPL(recording, v8::String)				EDF_HEADER_SET_ERROR_PROP_IMPL(recording)
EDF_HEADER_GET_STRING_PROP_IMPL(patientcode, v8::String)			EDF_HEADER_SET_STRING_PROP_IMPL(patientcode, v8::String, edf_set_patientcode)
EDF_HEADER_GET_STRING_PROP_IMPL(gender, v8::String)	
NAN_SETTER(EdfModule::prp_header_set_gender) {
		EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
		int v = To<int>(value).FromMaybe(0);
		if (self->isReadMode()) ThrowError(READ_ONLY_TXT); else
		{
			int err = edf_set_gender(self->edfInfo().handle, v);
			if (err) {
				std::stringstream ss;
				ss << EDF_LIB_ERROR << " " << err << " [" << edferror_text(err) << "]";
				ThrowError(ss.str().c_str());
			}
			else {
				if (v == 0) std::strcpy(self->edfInfo().gender, "female");
				else if (v == 1) std::strcpy(self->edfInfo().gender, "male");
			}
		}
		
		info.GetReturnValue().Set(info.This()); 
}

EDF_HEADER_GET_STRING_PROP_IMPL(birthdate, v8::String)
NAN_SETTER(EdfModule::prp_header_set_birthdate) {
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	if (self->isReadMode()) ThrowError(READ_ONLY_TXT); else
	{
		int day, month, year;
		char *v = *Utf8String(value);
		if (3 == sscanf(v, "%d.%d.%d", &day, &month, &year))
		{
			int err = edf_set_birthdate(self->edfInfo().handle, year, month, day);
			if (err) {
				std::stringstream ss;
				ss << EDF_LIB_ERROR << " " << err << " [" << edferror_text(err) << "]";
				ThrowError(ss.str().c_str());
			}
			else std::strncpy(self->edfInfo().birthdate, v, sizeof(self->edfInfo().birthdate));
		}
	}
	info.GetReturnValue().Set(info.This());
}
EDF_HEADER_GET_STRING_PROP_IMPL(patient_name, v8::String)					EDF_HEADER_SET_STRING_PROP_IMPL(patient_name, v8::String, edf_set_patientname)
EDF_HEADER_GET_STRING_PROP_IMPL(patient_additional, v8::String)				EDF_HEADER_SET_STRING_PROP_IMPL(patient_additional, v8::String, edf_set_patient_additional)
EDF_HEADER_GET_STRING_PROP_IMPL(admincode, v8::String)						EDF_HEADER_SET_STRING_PROP_IMPL(admincode, v8::String, edf_set_admincode)
EDF_HEADER_GET_STRING_PROP_IMPL(technician, v8::String)						EDF_HEADER_SET_STRING_PROP_IMPL(technician, v8::String, edf_set_technician)
EDF_HEADER_GET_STRING_PROP_IMPL(equipment, v8::String)						EDF_HEADER_SET_STRING_PROP_IMPL(equipment, v8::String, edf_set_equipment)
EDF_HEADER_GET_STRING_PROP_IMPL(recording_additional, v8::String)			EDF_HEADER_SET_STRING_PROP_IMPL(recording_additional, v8::String, edf_set_recording_additional)

EDF_HEADER_GET_INT_PROP_IMPL(datarecord_duration, v8::Number, double)		EDF_HEADER_SET_INT_PROP_IMPL(datarecord_duration, v8::Number, double, edf_set_datarecord_duration)
EDF_HEADER_GET_INT_PROP_IMPL(datarecords_in_file, v8::Number, double)		EDF_HEADER_SET_ERROR_PROP_IMPL(datarecords_in_file)
EDF_HEADER_GET_INT_PROP_IMPL(annotations_in_file, v8::Number, double)		EDF_HEADER_SET_ERROR_PROP_IMPL(annotations_in_file)

EDFLIB_IMPL_DEFINE_GETTER(EDFLIB_DO_NOT_READ_ANNOTATIONS)
EDFLIB_IMPL_DEFINE_GETTER(EDFLIB_READ_ANNOTATIONS)
EDFLIB_IMPL_DEFINE_GETTER(EDFLIB_READ_ALL_ANNOTATIONS)
EDFLIB_IMPL_DEFINE_GETTER(EDFSEEK_SET)
EDFLIB_IMPL_DEFINE_GETTER(EDFSEEK_CUR)
EDFLIB_IMPL_DEFINE_GETTER(EDFSEEK_END)

EDFLIB_IMPL_DEFINE_GETTER(EDFLIB_FILETYPE_EDF)
EDFLIB_IMPL_DEFINE_GETTER(EDFLIB_FILETYPE_EDFPLUS)
EDFLIB_IMPL_DEFINE_GETTER(EDFLIB_FILETYPE_BDF)
EDFLIB_IMPL_DEFINE_GETTER(EDFLIB_FILETYPE_BDFPLUS)

/*
NAN_GETTER(EdfModule::prp_header_get_patient) {
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This()); 
	MaybeLocal<v8::String> result = Nan::New(self->m_edfInfo.patient);
	info.GetReturnValue().Set(result.ToLocalChecked()); }
NAN_SETTER(EdfModule::prp_header_set_patient) {
	EdfModule *self = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	//Local<v8::String> s = To<v8::String>(value).FromMaybe(Local<v8::String>());
	std::strncpy(
		self->m_edfInfo.patient
		, *Utf8String(value)
		, sizeof(self->m_edfInfo.patient));
	self->m_edfInfo.patient[sizeof(self->m_edfInfo.patient) - 1] = '\0';
	info.GetReturnValue().Set(info.This()); }
	*/
NAN_METHOD(EdfModule::New) {
	if (info.IsConstructCall()) {
		EdfModule* obj = new EdfModule();
		std::stringstream ss;
		ss << "uid_" << next_gc();
		//obj->m_object_uid = ss.str();
		obj->Wrap(info.This());
		info.GetReturnValue().Set(info.This());
	}
	else {
		v8::Local<v8::Function> cons = Nan::New<v8::Function>(constructor);
		info.GetReturnValue().Set(cons->NewInstance());
	}
}


NAN_GETTER(EdfModule::signalparam_getter)
{
	EdfModule* obj = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	if (!obj->m_fileOpened)
	{
		ThrowError("File not opened");
		return;
	}
	Local<v8::Array> results = Nan::New<v8::Array>(obj->edfInfo().edfsignals);
	for (int i = 0; i < obj->edfInfo().edfsignals; i++)
	{
		edf_param_struct &p = obj->edfInfo().signalparam[i];
		Local<v8::Object> itm = Nan::New<v8::Object>();
		itm->Set(Nan::New("label").ToLocalChecked(), Nan::New(trim_str(p.label)).ToLocalChecked());
		itm->Set(Nan::New("smp_in_file").ToLocalChecked(), Nan::New(static_cast<double>(p.smp_in_file)));
		itm->Set(Nan::New("phys_max").ToLocalChecked(), Nan::New(p.phys_max));
		itm->Set(Nan::New("phys_min").ToLocalChecked(), Nan::New(p.phys_min));
		itm->Set(Nan::New("dig_max").ToLocalChecked(), Nan::New(p.dig_max));
		itm->Set(Nan::New("dig_min").ToLocalChecked(), Nan::New(p.dig_min));
		itm->Set(Nan::New("smp_in_datarecord").ToLocalChecked(), Nan::New(p.smp_in_datarecord));
		itm->Set(Nan::New("physdimension").ToLocalChecked(), Nan::New(trim_str(p.physdimension)).ToLocalChecked());
		itm->Set(Nan::New("prefilter").ToLocalChecked(), Nan::New(trim_str(p.prefilter)).ToLocalChecked());
		itm->Set(Nan::New("transducer").ToLocalChecked(), Nan::New(trim_str(p.transducer)).ToLocalChecked());
		Nan::Set(results, i, itm);
	}
	info.GetReturnValue().Set(results);
}


NAN_GETTER(EdfModule::annotations_getter)
{
	EdfModule* obj = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	if (!obj->m_fileOpened)
	{
		ThrowError("File not opened");
		return;
	}
	Local<v8::Array> results = Nan::New<v8::Array>(obj->edfInfo().annotations_in_file);
	edf_annotation_struct annot;
	for (int i = 0; i < obj->edfInfo().annotations_in_file; i++)
	{
		std::memset(&annot, 0, sizeof(annot));
		int err = edf_get_annotation(obj->edfInfo().handle, i, &annot);
		if (err < 0) {
			std::stringstream ss;
			ss << "Error obtaining annotation #" << i << " Edf error " << err << " [" << edferror_text(err) << "]";
			ThrowError(ss.str().c_str());
			return;
		}
		Local<v8::Object> itm = Nan::New<v8::Object>();
		itm->Set(Nan::New("onset").ToLocalChecked(), Nan::New(static_cast<double>(annot.onset)));
		itm->Set(Nan::New("duration").ToLocalChecked(), Nan::New(annot.duration).ToLocalChecked());
		itm->Set(Nan::New("annotation").ToLocalChecked(), Nan::New(trim_str(annot.annotation).c_str()).ToLocalChecked());
		Nan::Set(results, i, itm);
	}
	info.GetReturnValue().Set(results);
}


/*void Method(const Nan::FunctionCallbackInfo<v8::Value>& info) {
	std::stringstream ss;
	ss << "edflib version " << edflib_version();
	info.GetReturnValue().Set(Nan::New(ss.str()).ToLocalChecked());
}*/

void Init(v8::Local<v8::Object> exports) {
  //exports->Set(Nan::New("hello").ToLocalChecked(),
  //             Nan::New<v8::FunctionTemplate>(Method)->GetFunction());
	EdfModule::Init(exports);
}

NODE_MODULE(edflib, Init)
