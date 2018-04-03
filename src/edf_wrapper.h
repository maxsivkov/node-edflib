#ifndef EDF_WRAPPER_H_
#define EDF_WRAPPER_H_

#pragma once


#include <nan.h>
#include <nan-check.h>
#include <string>
#include <iostream>
#include <locale>
#include <edflib.h>
#include <thread>
#include <mutex>
#include <sstream>
#include <memory>

#ifdef max
#undef max
#endif

using Nan::AsyncWorker;
using Nan::Callback;
using Nan::GetFunction;
using Nan::HandleScope;
using Nan::New;
using Nan::Null;
using Nan::Set;

using v8::Function;
using v8::FunctionTemplate;
using v8::Local;
using v8::String;
using v8::Value;

using namespace Nan;

#define FILE_NOT_OPENED_TXT "File not opened"
#define WRONG_ARGUMENTS_TXT "Wrong arguments"
#define READ_ONLY_TXT "File read only"
#define NOT_IMPLEMENTED_TXT "Not implemented"
#define ERR_WRONG_SIGNAL_LENGTH "Wrong length of signals block"
#define ERR_NOT_ARRAY "Array expected"

#define EDF_LIB_ERROR "EdfLib error"

enum ReadAnnotations{
	DoNotRead = EDFLIB_DO_NOT_READ_ANNOTATIONS
	, Read = EDFLIB_READ_ANNOTATIONS
	, ReadAll = EDFLIB_READ_ALL_ANNOTATIONS
};
enum StartdayFields : unsigned char {
	ST_INITIAL = 1 << 0
	, ST_startdate_day = 1 << 1
	, ST_startdate_month = 1 << 2
	, ST_startdate_year = 1 << 3
	, ST_starttime_subsecond = 1 << 4
	, ST_starttime_second = 1 << 5
	, ST_starttime_minute = 1 << 6
	, ST_starttime_hour = 1 << 7
	, ST_MASK = 0xFF
};

#define EDF_HEADER_PROP(Name) \
		static NAN_GETTER(prp_header_get_##Name);\
		static NAN_SETTER(prp_header_set_##Name)
#define EDFLIB_DEFINE_GETTER(Name) \
	static NAN_GETTER(const_getter##Name)


template< typename C, typename T, typename A >
std::basic_string<C, T, A> trim(const std::basic_string<C, T, A>& str,
	const std::locale& loc = std::locale::classic())
{
	auto begin = str.begin(); // first non ws from left
	while (begin != str.end() && std::isspace(*begin, loc)) ++begin;
	if (begin == str.end()) return{}; // EDIT

	auto rbegin = str.rbegin(); // first non ws from right
	while (rbegin != str.rend() && std::isspace(*rbegin, loc)) ++rbegin;

	return{ begin, rbegin.base() };
}

std::string trim_str(const char*s);

class EdfModule : public node::ObjectWrap {
private:
	static Persistent<Function> constructor;
	static Persistent<Function> m_loggerCb;
public:
	typedef std::mutex edf_lock_t;
	typedef std::lock_guard<edf_lock_t> edf_lock_guard_t;
	
	enum OpenMode {NoneMode, ReadMode, WriteMode};
	void log(const std::ostream &);
	struct signal_info_t {
		int m_samplefrequency;
		double m_phys_min, m_phys_max;
		int m_digital_min, m_digital_max;

		signal_info_t() : m_samplefrequency(0), m_phys_min(0.0), m_phys_max(0.0), m_digital_min(0), m_digital_max(0){}
	};
	typedef std::map<int, signal_info_t> write_signals_info_t;

	int n_currentWriteSignalNo;
	edf_lock_t m_edfLock;
	OpenMode	m_openMode;
	write_signals_info_t m_write_signals_info;
	edf_hdr_struct m_edfInfo;
	bool	m_fileOpened;
	unsigned int	m_startdaySetters;

	void clear();
	bool isReadMode() const { return m_openMode == ReadMode; }
	bool isWriteMode() const { return m_openMode == WriteMode; }

	static NAN_MODULE_INIT(Init);
	static NAN_METHOD(New);
	static NAN_METHOD(GetVersion);
	static NAN_METHOD(SetLogger);

	static NAN_GETTER(signalparam_getter);
	static NAN_GETTER(annotations_getter);

	EDFLIB_DEFINE_GETTER(EDFLIB_DO_NOT_READ_ANNOTATIONS);
	EDFLIB_DEFINE_GETTER(EDFLIB_READ_ANNOTATIONS);
	EDFLIB_DEFINE_GETTER(EDFLIB_READ_ALL_ANNOTATIONS);

	EDFLIB_DEFINE_GETTER(EDFSEEK_SET);
	EDFLIB_DEFINE_GETTER(EDFSEEK_CUR);
	EDFLIB_DEFINE_GETTER(EDFSEEK_END);

	EDFLIB_DEFINE_GETTER(EDFLIB_FILETYPE_EDF);
	EDFLIB_DEFINE_GETTER(EDFLIB_FILETYPE_EDFPLUS);
	EDFLIB_DEFINE_GETTER(EDFLIB_FILETYPE_BDF);
	EDFLIB_DEFINE_GETTER(EDFLIB_FILETYPE_BDFPLUS);


	static NAN_METHOD(EdfSeek);
	static NAN_METHOD(EdfTell);
	static NAN_METHOD(EdfRewind);

	static NAN_METHOD(SetSampleFrequency);
	static NAN_METHOD(SetPhysicalParameters);
	static NAN_METHOD(SetDigitalParameters);
	static Nan::NAN_METHOD_RETURN_TYPE WriteAnnotation(Nan::NAN_METHOD_ARGS_TYPE, std::function<int(int, int64_t, int64_t, const char *)>);
	
	static NAN_METHOD(WriteAnnotationUtf8);
	static NAN_METHOD(WriteAnnotationLatin1);
	static NAN_METHOD(SetNumberOfAnnotationSignals);
	
	static Nan::NAN_METHOD_RETURN_TYPE SetSignalStringProp(Nan::NAN_METHOD_ARGS_TYPE, std::function<int(int, int, const char *)>);
	static NAN_METHOD(SetLabel);
	static NAN_METHOD(SetPrefilter);
	static NAN_METHOD(SetTransducer);
	static NAN_METHOD(SetPhysicalDimension);

	/*static NAN_METHOD(prp_Header);
	static NAN_GETTER(prp_GetHeader);
	static NAN_SETTER(prp_SetHeader);
	*/
	EDF_HEADER_PROP(handle);
	EDF_HEADER_PROP(filetype);
	EDF_HEADER_PROP(edfsignals);
	EDF_HEADER_PROP(file_duration);
	EDF_HEADER_PROP(startdate_day);
	EDF_HEADER_PROP(startdate_month);
	EDF_HEADER_PROP(startdate_year);
	EDF_HEADER_PROP(starttime_subsecond);
	EDF_HEADER_PROP(starttime_second);
	EDF_HEADER_PROP(starttime_minute);
	EDF_HEADER_PROP(starttime_hour);
	
	EDF_HEADER_PROP(patient);
	EDF_HEADER_PROP(recording);
	EDF_HEADER_PROP(patientcode);
	EDF_HEADER_PROP(gender);
	EDF_HEADER_PROP(birthdate);
	EDF_HEADER_PROP(patient_name);
	EDF_HEADER_PROP(patient_additional);
	EDF_HEADER_PROP(admincode);
	EDF_HEADER_PROP(technician);
	EDF_HEADER_PROP(equipment);
	EDF_HEADER_PROP(recording_additional);
	EDF_HEADER_PROP(datarecord_duration);
	EDF_HEADER_PROP(datarecords_in_file);
	EDF_HEADER_PROP(annotations_in_file);
	//EDF_HEADER_PROP();
	
	inline edf_hdr_struct& edfInfo() {
		return m_edfInfo;
	};
	EdfModule();
	~EdfModule();
};

class EdfHeaderWrapper : node::ObjectWrap, edf_hdr_struct {
	static NAN_GETTER(prp_GetHeader);
};


class EdfOpenReadWorker: Nan::AsyncWorker {
public:  
	EdfOpenReadWorker(Nan::Callback *, EdfModule &);
	~EdfOpenReadWorker();
	void Execute();
	std::string m_path, m_result, m_error;
	int m_readAnnotations;
	EdfModule &m_module;
	static NAN_METHOD(OpenRead);

protected:  
	void HandleOKCallback();
};

class EdfOpenWriteWorker : Nan::AsyncWorker {
public:
	EdfOpenWriteWorker(Nan::Callback *, EdfModule &);
	~EdfOpenWriteWorker();
	void Execute();
	std::string m_path, m_result, m_error;
	int m_fileType, m_signalsNo;
	EdfModule &m_module;
	static NAN_METHOD(OpenWrite);

protected:
	void HandleOKCallback();
};



class EdfCloseFileWorker : Nan::AsyncWorker {
public:
	EdfCloseFileWorker(Nan::Callback *, EdfModule &);
	~EdfCloseFileWorker();
	EdfModule &m_module;
	std::string m_error;
	void Execute();
	static NAN_METHOD(CloseFile);

protected:
	void HandleOKCallback();
};


class EdfReadPhysicalSamplesWorker : Nan::AsyncWorker {
public:
	EdfReadPhysicalSamplesWorker(Nan::Callback *, EdfModule &, const int&, const int & );
	~EdfReadPhysicalSamplesWorker();
	void Execute();
	EdfModule &m_module;
	int m_signal, m_number;
	std::string m_error;
	typedef std::vector<double> buf_t;
	buf_t m_buf;
	static NAN_METHOD(ReadPhysicalSamples);

protected:
	void HandleOKCallback();
};

class EdfReadDigitalSamplesWorker : Nan::AsyncWorker {
public:
	EdfReadDigitalSamplesWorker(Nan::Callback *, EdfModule &, const int&, const int &);
	~EdfReadDigitalSamplesWorker();
	void Execute();
	EdfModule &m_module;
	int m_signal, m_number;
	std::string m_error;
	typedef std::vector<int> buf_t;
	buf_t m_buf;
	static NAN_METHOD(ReadDigitalSamples);

protected:
	void HandleOKCallback();
};

class EdfReadAnnotationsWorker : Nan::AsyncWorker {
public:
	EdfReadAnnotationsWorker(Nan::Callback *, EdfModule &, const int &);
	~EdfReadAnnotationsWorker();
	void Execute();
	EdfModule &m_module;
	typedef edf_annotation_struct item_t;
	typedef std::vector<item_t> items_t;
	items_t m_items;
	int m_number;
	std::string m_error;
	static NAN_METHOD(ReadAnnotations);

protected:
	void HandleOKCallback();
};

class WriteSamplesWorker : Nan::AsyncWorker {
	template <typename T> struct data_holder_t {
		typedef std::vector<T> block_t;
		block_t m_block;
		uint32_t m_block_len;
		void item_push_back(const T&v) { m_block.push_back(v); }
	};
	struct edf_write_handler_t {
		int m_edfResult;
		EdfModule &m_module;
		std::string m_error, m_result;
		edf_write_handler_t(EdfModule &module) : m_edfResult(0)
			, m_module(module), m_error(""), m_result("OK"){}
		virtual ~edf_write_handler_t() {}
		virtual void init(Local<v8::Array> &signals_data) = 0;
		//virtual void init_block_from_buf(Local<v8::Array> &signals_data) = 0;
		//virtual void init_block_from_array(const Local<v8::Array> &signals_data) = 0;
		void write() {
			m_module.m_edfLock.lock();
			edf_write();
			m_module.m_edfLock.unlock();
			if (m_edfResult < 0) {
				std::stringstream ss;
				ss << EDF_LIB_ERROR << " signal #" << m_module.n_currentWriteSignalNo % m_module.m_write_signals_info.size() << " error # " << m_edfResult << " [" << edferror_text(m_edfResult) << "]";
				m_error = ss.str();
				//m_result = "";
			}
			else {
				//std::stringstream ss;
				//ss << m_items.size();
				//m_result = ss.str();
			}
		}
	protected:
		virtual void edf_write() = 0;
	}; //edf_write_handler_t
	
	template <typename T, typename CastT = T> struct samples_t : public edf_write_handler_t, public data_holder_t<T> {
		typedef T item_t;
		size_t m_block_len;

		samples_t(EdfModule &module) : edf_write_handler_t(module), m_block_len(0){}
		void init_block_from_buf(Local<v8::Array> &signals_data) {
			for (uint32_t i = 0; i < signals_data->Length(); i++)
				this->item_push_back(To<CastT>(signals_data->Get(i)).FromJust());
		}
		void init_block_from_array(const Local<v8::Array> &signals_data)
		{
			typedef std::vector<size_t> indecies_t;
			indecies_t indecies = indecies_t(signals_data->Length(), 0);

			uint32_t min_blocks = std::numeric_limits<uint32_t>::max(), max_blocks = 0;
			// perform misc checks

			for (size_t signal_no = 0; signal_no < signals_data->Length(); signal_no++) {
				EdfModule::signal_info_t &sg_info = m_module.m_write_signals_info.at(signal_no);
				if (signals_data->Get(signal_no)->IsArray())
				{
					Local<v8::Array> signal_data_buf = Local<v8::Array>::Cast(signals_data->Get(signal_no));
					uint32_t nblocks = signal_data_buf->Length() / sg_info.m_samplefrequency;
					m_block_len += sg_info.m_samplefrequency;
					if (nblocks > max_blocks) max_blocks = nblocks;
					if (nblocks < min_blocks) min_blocks = nblocks;
					if (signal_data_buf->Length() % sg_info.m_samplefrequency) {
						m_module.log(std::ostringstream() << "BLKWR Wrong length of signal #" << signal_no << " data length " << signals_data->Length() << " samplefrequency " << sg_info.m_samplefrequency);
						throw std::logic_error(ERR_WRONG_SIGNAL_LENGTH);
					}
				}
				else throw std::logic_error(ERR_NOT_ARRAY);
			}
			if (min_blocks != max_blocks || (min_blocks == max_blocks && min_blocks == 0))
			{
				m_module.log(std::ostringstream() << "BLKWR Wrong block length min " << min_blocks << " max " << max_blocks);
				throw std::logic_error(ERR_WRONG_SIGNAL_LENGTH);
			}

			size_t iterations = 0;
			while (iterations < signals_data->Length() * min_blocks) {
				//for (int signal_no = 0; signal_no < signals_data->Length(); signal_no++) {
				uint32_t signal_no = iterations % signals_data->Length();
				EdfModule::signal_info_t &sg_info = m_module.m_write_signals_info.at(signal_no);
				size_t &data_index = indecies.at(signal_no);
				size_t data_index_end = data_index + sg_info.m_samplefrequency;
				Local<v8::Array> signal_data_buf = Local<v8::Array>::Cast(signals_data->Get(signal_no));

				m_module.log(std::ostringstream() << "BLKWR sig#" << signal_no << " data index " << data_index << " data_index_end " << data_index_end);

				for (; data_index < data_index_end; data_index++)
					this->item_push_back(To<CastT>(signal_data_buf->Get(data_index)).FromJust());


				iterations++;
			}
		};
	};
	typedef samples_t<double> data_holder_double_t;
	typedef samples_t<int> data_holder_int_t;
	typedef samples_t<short, int> data_holder_short_t;
	typedef samples_t<int> data_holder_int_void_t;
	
	template <typename T, typename item_t = typename T::item_t > struct edf_buf_writer : T {
		typedef T parent_t;
		typedef std::function<int(int, item_t*)> edf_function_t;
		edf_function_t m_function;
		edf_buf_writer(EdfModule &module, edf_function_t edf_function) : parent_t(module), m_function(edf_function) {}
		void edf_write_buf() {
			this->m_edfResult = m_function(this->m_module.edfInfo().handle, this->m_block.data());
			if (!this->m_edfResult) this->m_module.n_currentWriteSignalNo++;
		}
		void edf_write_block() {
			std::stringstream ss;
			ss << "writing " << this->m_block.size() / this->m_block_len << " blocks data size " 
                << this->m_block.size() << " m_block_len " << this->m_block_len;
			//m_result = ss.str();

			for (uint32_t block_no = 0; block_no < this->m_block.size() / this->m_block_len; block_no++)
			{
				this->m_edfResult = m_function(this->m_module.edfInfo().handle, this->m_block.data() + block_no * this->m_block_len);
				if (this->m_edfResult < 0) break;
			}
		}
	};


	// WritePhysicalSamples edfwrite_physical_samples call
	struct write_phys_samples_t : edf_buf_writer<data_holder_double_t> {
		typedef edf_buf_writer<data_holder_double_t> parent_t;
		write_phys_samples_t(EdfModule &module) : parent_t(module, edfwrite_physical_samples) {}
		virtual void init(Local<v8::Array> &signals_data) { init_block_from_buf(signals_data); }
	protected:
		virtual void edf_write() { edf_write_buf();}
	}; //write_phys_samples_t
	
	// BlockWritePhysicalSamples edf_blockwrite_physical_samples call
	struct block_write_phys_samples_t : public edf_buf_writer<data_holder_double_t> {
		typedef edf_buf_writer<data_holder_double_t> parent_t;
		block_write_phys_samples_t(EdfModule &module) : parent_t(module, edf_blockwrite_physical_samples) {}
		virtual void init(Local<v8::Array> &signals_data) { init_block_from_array(signals_data); }
	protected:
		virtual void edf_write() { edf_write_block(); }
	}; //block_write_phys_samples_t

	// WriteDigitalShortSamples edfwrite_digital_short_samples call
	struct write_digital_short_samples_t : edf_buf_writer<data_holder_short_t> {
		typedef edf_buf_writer<data_holder_short_t> parent_t;
		write_digital_short_samples_t(EdfModule &module) : parent_t(module, edfwrite_digital_short_samples) {}
		virtual void init(Local<v8::Array> &signals_data) { init_block_from_buf(signals_data); }
	protected:
		virtual void edf_write() { edf_write_buf(); }
	}; //write_digital_short_samples_t

	   // WriteDigitalSamples edfwrite_digital_samples call
	struct write_digital_samples_t : edf_buf_writer<data_holder_int_t> {
		typedef edf_buf_writer<data_holder_int_t> parent_t;
		write_digital_samples_t(EdfModule &module) : parent_t(module, edfwrite_digital_samples) {}
		virtual void init(Local<v8::Array> &signals_data) { init_block_from_buf(signals_data); }
	protected:
		virtual void edf_write() { edf_write_buf(); }
	}; //write_digital_short_samples_t

	// BlockWriteDigitalShortSamples edf_blockwrite_digital_short_samples call
	struct block_write_digital_short_samples_t : public edf_buf_writer<data_holder_short_t> {
		typedef edf_buf_writer<data_holder_short_t> parent_t;
		block_write_digital_short_samples_t(EdfModule &module) : parent_t(module, edf_blockwrite_digital_short_samples) {}
		virtual void init(Local<v8::Array> &signals_data) { init_block_from_array(signals_data); }
	protected:
		virtual void edf_write() { edf_write_block(); }
	}; //block_write_digital_short_samples_t

	// BlockWriteDigitalSamples edf_blockwrite_digital_samples call
	struct block_write_digital_samples_t : public edf_buf_writer<data_holder_int_t> {
		typedef edf_buf_writer<data_holder_int_t> parent_t;
		block_write_digital_samples_t(EdfModule &module) : parent_t(module, edf_blockwrite_digital_samples) {}
		virtual void init(Local<v8::Array> &signals_data) { init_block_from_array(signals_data); }
	protected:
		virtual void edf_write() { edf_write_block(); }
	}; //block_write_digital_samples_t
	
	// BlockWriteDigital3ByteSamples edf_blockwrite_digital_3byte_samples call
	struct block_write_digital_3byte_samples_t : public edf_buf_writer<data_holder_int_t, void> {
		typedef edf_buf_writer<data_holder_int_t, void> parent_t;
		block_write_digital_3byte_samples_t(EdfModule &module) : parent_t(module, edf_blockwrite_digital_3byte_samples) {}
		virtual void init(Local<v8::Array> &signals_data) { init_block_from_array(signals_data); }
	protected:
		virtual void edf_write() { edf_write_block(); }
	}; //block_write_digital_3byte_samples_t
public:
	WriteSamplesWorker(Nan::Callback *, EdfModule &);
	~WriteSamplesWorker();
	void Execute();
	typedef std::unique_ptr<edf_write_handler_t> write_handler_t;
	write_handler_t m_writeHandler;
	EdfModule &m_module;
	typedef std::vector<double> items_t;
	items_t m_items;
	std::string m_error, m_result;
	enum Operation {
		OP_WritePhysicalSamples
		, OP_BlockWritePhysicalSamples
	};
	Operation m_operation;
	static NAN_METHOD(WritePhysicalSamples);
	static NAN_METHOD(BlockWritePhysicalSamples);
	static NAN_METHOD(WriteDigitalShortSamples);
	static NAN_METHOD(WriteDigitalSamples);
	static NAN_METHOD(BlockWriteDigitalShortSamples);
	static NAN_METHOD(BlockWriteDigitalSamples);
	static NAN_METHOD(BlockWriteDigital3ByteSamples);

	template <typename HandlerType> 
	static NAN_METHOD(WriteBufSamples);
	template <typename HandlerType>
	static NAN_METHOD(WriteBlockSamples);
protected:
	void HandleOKCallback();
	static bool basicChecks(EdfModule*, Nan::NAN_METHOD_ARGS_TYPE info, Local<v8::Array> &buf, Local<Function> &callback, std::string &error
			, std::function<bool(Local<v8::Array>&)> additional_check);
};

template <typename HandlerType>
NAN_METHOD(WriteSamplesWorker::WriteBufSamples){
	Local<Function> _callback;
	EdfModule* obj = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	std::string error = WRONG_ARGUMENTS_TXT;

	try {
		uint32_t signal_no = obj->n_currentWriteSignalNo % obj->m_write_signals_info.size();
		EdfModule::signal_info_t &sg_info = obj->m_write_signals_info.at(signal_no);
		Local<v8::Array> buf = New<v8::Array>(0);

		if (basicChecks(obj, info, buf, _callback, error, [&sg_info, &obj, &signal_no](Local<v8::Array>&data_buf) {
			bool result = (int)data_buf->Length() == sg_info.m_samplefrequency;
			if (!result)
				obj->log(std::ostringstream() << "WR Wrong data length for signal #" << signal_no 
                    << " data length " << data_buf->Length() << " samplefrequency " << sg_info.m_samplefrequency);

			return result;
		}))
		{
			WriteSamplesWorker::write_handler_t writeHandler(new HandlerType(*obj));
			writeHandler->init(buf);

			WriteSamplesWorker *worker = new WriteSamplesWorker(new Callback(_callback), *obj);
			worker->m_writeHandler = std::move(writeHandler);
			AsyncQueueWorker(worker);
		}
	}
	catch (std::logic_error &err) {
		ThrowError(err.what());
	}
}
template <typename HandlerType>
NAN_METHOD(WriteSamplesWorker::WriteBlockSamples) {
	Local<Function> _callback;
	EdfModule* obj = node::ObjectWrap::Unwrap<EdfModule>(info.This());
	std::string error = WRONG_ARGUMENTS_TXT;
	try {
		Local<v8::Array> signals_data = New<v8::Array>(0);
		if (basicChecks(obj, info, signals_data, _callback, error, [&obj](Local<v8::Array>&data_buf) {
			bool result = data_buf->IsArray() && data_buf->Length() > 0 && data_buf->Length() == obj->m_write_signals_info.size();
			if (!result)
				obj->log(std::ostringstream() << WRONG_ARGUMENTS_TXT << data_buf->IsArray() << " " << data_buf->Length());

			return result;
		}))
		{
			WriteSamplesWorker::write_handler_t writeHandler(new HandlerType(*obj));
			writeHandler->init(signals_data);
			WriteSamplesWorker *worker = new WriteSamplesWorker(new Callback(_callback), *obj);
			worker->m_writeHandler = std::move(writeHandler);

			AsyncQueueWorker(worker);
		}
	}
	catch (std::logic_error &err) {
		ThrowError(err.what());
	}
}
#endif
