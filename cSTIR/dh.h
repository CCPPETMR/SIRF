#ifndef DATA_HANDLE
#define DATA_HANDLE

#include <stdlib.h>

#include <boost\algorithm\string.hpp>

#include "StirException.h"

#define NEW(T, X) T* X = new T
#define CAST_PTR(T, X, Y) T* X = (T*)Y
#define NEW_SPTR(Base, X, Object) \
	boost::shared_ptr< Base >* X = new boost::shared_ptr< Base >(new Object)

class ExecutionStatus {
public:
	ExecutionStatus() : _error(0), _file(0), _line(0) {}
	ExecutionStatus(const char* error, const char* file, int line) {
		set(error, file, line);
	}
	ExecutionStatus(const ExecutionStatus& s) {
		set(s.error(), s.file(), s.line());
	}
	ExecutionStatus(const StirException& ex) {
		set(ex.what(), ex.file(), ex.line());
	}
	~ExecutionStatus() {
		delete[] _error;
		delete[] _file;
	}
	const char* error() const { return _error; }
	const char* file() const { return _file; }
	int line() const { return _line; }
private:
	char* _error;
	char* _file;
	int _line;
	void set(const char* error, const char* file, int line) {
		size_t size;
		if (error) {
			size = strlen(error) + 1;
			_error = new char[size];
			memcpy(_error, error, size);
		}
		else
			_error = 0;
		if (file) {
			size = strlen(file) + 1;
			_file = new char[size];
			memcpy(_file, file, size);
		}
		else
			_file = 0;
		_line = line;
	}
};

class DataHandle { 
public:
	DataHandle() : _data(0), _status(0), _owns_data(0) {} 
	virtual ~DataHandle() {
		if (_data && _owns_data)
			free(_data);
		delete _status;
	}
	void set(void* data, const ExecutionStatus* status = 0, int grab = 0) {
		if (status) {
			delete _status;
			_status = new ExecutionStatus(*status);
		}
		if (_data && _owns_data)
			free(_data);
		_data = data;
		_owns_data = grab != 0;
	}
	void* data() const { return _data; }
	const ExecutionStatus* status() const { return _status; }
protected:
	bool _owns_data;
	void* _data;
	ExecutionStatus* _status;
};

class anObjectHandle : public DataHandle {
public:
	virtual ~anObjectHandle() {}
	virtual anObjectHandle* copy() = 0;
};

template<class Base>
class ObjectHandle : public anObjectHandle {
public:
	ObjectHandle(const ObjectHandle& obj) { 
		NEW(boost::shared_ptr<Base>, ptr_sptr);
		*ptr_sptr = *(boost::shared_ptr<Base>*)obj.data();
		_data = (void*)ptr_sptr;
		if (obj._status)
			_status = new ExecutionStatus(*obj._status);
		else
			_status = 0;
	}
	ObjectHandle(const boost::shared_ptr<Base>& sptr,
		const ExecutionStatus* status = 0) { 
		NEW(boost::shared_ptr<Base>, ptr_sptr);
		*ptr_sptr = sptr;
		_data = (void*)ptr_sptr;
		if (status)
			_status = new ExecutionStatus(*status);
		else
			_status = 0;
	}
	virtual ~ObjectHandle() {
		CAST_PTR(boost::shared_ptr<Base>, ptr_sptr, _data);
		delete _status;
		delete ptr_sptr;
	}
	virtual anObjectHandle* copy() {
		if (_data == 0)
			throw StirException("zero data pointer cannot be dereferenced",
			__FILE__, __LINE__);
		CAST_PTR(boost::shared_ptr<Base>, ptr_sptr, _data);
		if (is_null_ptr(*ptr_sptr))
			throw StirException("zero object pointer cannot be dereferenced",
			__FILE__, __LINE__);
		return new ObjectHandle<Base>(*ptr_sptr, _status);
	}
};

template<class Base, class Object>
static void*
newObjectHandle()
{
	NEW_SPTR(Base, ptr_sptr, Object);
	ObjectHandle<Base>* ptr_handle = new ObjectHandle<Base>(*ptr_sptr);
	return (void*)ptr_handle;
}

template<class Base>
static void*
newObjectHandle(boost::shared_ptr<Base>* ptr_sptr)
{
	ObjectHandle<Base>* ptr_handle = new ObjectHandle<Base>(*ptr_sptr);
	return (void*)ptr_handle;
}

template<class T>
void*
sptrObjectHandle(boost::shared_ptr<T> sptr) {
	NEW(boost::shared_ptr<T>, ptr_sptr);
	*ptr_sptr = sptr;
	ObjectHandle<T>* ptr_handle = new ObjectHandle<T>(*ptr_sptr);
	return (void*)ptr_handle;
}

template<class Base>
Base&
objectFromHandle(const DataHandle* handle) {
	void* ptr = handle->data();
	if (ptr == 0)
		throw StirException("zero data pointer cannot be dereferenced",
		__FILE__, __LINE__);
	CAST_PTR(boost::shared_ptr<Base>, ptr_sptr, ptr);
	if (is_null_ptr(*ptr_sptr))
		throw StirException("zero object pointer cannot be dereferenced",
		__FILE__, __LINE__);
	CAST_PTR(Base, ptr_object, ptr_sptr->get());
	return *ptr_object;
}

template<class Base, class Object>
Object&
objectFromHandle(const DataHandle* handle) {
	void* ptr = handle->data();
	if (ptr == 0)
		throw StirException("zero data pointer cannot be dereferenced",
		__FILE__, __LINE__);
	CAST_PTR(boost::shared_ptr<Base>, ptr_sptr, ptr);
	if (is_null_ptr(*ptr_sptr))
		throw StirException("zero object pointer cannot be dereferenced",
		__FILE__, __LINE__);
	CAST_PTR(Object, ptr_object, ptr_sptr->get());
	return *ptr_object;
}

template<class Base>
boost::shared_ptr<Base>&
objectSptrFromHandle(const DataHandle* handle) {
	void* ptr = handle->data();
	if (ptr == 0)
		throw StirException("zero data pointer cannot be dereferenced",
		__FILE__, __LINE__);
	CAST_PTR(boost::shared_ptr<Base>, ptr_sptr, ptr);
	if (is_null_ptr(*ptr_sptr))
		throw StirException("zero object pointer cannot be dereferenced",
		__FILE__, __LINE__);
	return *ptr_sptr;
}

template<class Base>
Base*
objectPtrFromHandle(const DataHandle* handle) {
	if (handle == 0)
		return 0;
	void* ptr = handle->data();
	if (ptr == 0)
		return 0;
	CAST_PTR(boost::shared_ptr<Base>, ptr_sptr, ptr);
	if (is_null_ptr(*ptr_sptr))
		return 0;
	return ptr_sptr->get();
}

template<class T>
boost::shared_ptr<T>
sptrDataFromHandle(const DataHandle* handle) {
	return *(boost::shared_ptr<T>*)handle->data();
}

char* charDataFromHandle(const DataHandle* ptr_h);

#define GRAB 1

template <typename T>
void
setDataHandle(DataHandle* h, T x)
{
	T* ptr = (T*)malloc(sizeof(T));
	*ptr = x;
	h->set((void*)ptr, 0, GRAB);
}

template <typename T>
void*
dataHandle(T x)
{
	DataHandle* h = new DataHandle;
	setDataHandle<T>(h, x);
	return (void*)h;
}

template <typename T>
T
dataFromHandle(const void* ptr)
{
	DataHandle* ptr_h = (DataHandle*)ptr;
	void* ptr_d = ptr_h->data();
	if (!ptr_d)
		return 0;
	else
		return *((T*)ptr_d);
}

#endif