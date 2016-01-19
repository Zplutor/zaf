#pragma once

namespace zaf {

template<typename Type>
class Nullable {
public:
	/**
	 Initialize the instance that doesn't have value. 
	 */
	Nullable() : has_value_(false) { }

	/**
	 Initialize the instance that has specified value.
	 */
	Nullable(const Type& value) {
		ConstructValue(value);
	}

	/**
	 Initialize the instance that has the same value as another.
	 */
	Nullable(const Nullable<Type>& other) : has_value_(other.HasValue()) {
		if (HasValue()) {
			ConstructValue(other.Value());
		}
	}

	/**
	 Destroy the instance.
	 */
	~Nullable() {
		Clear();
	}

	/**
	 Assign the instance with the specified value.
	 */
	Nullable<Type>& operator=(const Type& value) {

		Clear();
		ConstructValue(value);
		return *this;
	}

	/**
	 Assign the instance with another.
	 */
	const Nullable<Type>& operator=(const Nullable<Type>& other) {

		Clear();
		if (other.HasValue()) {
			ConstructValue(other.Value());
		}
		return *this;
	}

	/**
	 Determinte whether the instance has value.
	 */
	bool HasValue() const {
		return has_value_;
	}

	/**
	 Get const reference to the value.
	 */
	const Type& GetValue() const {
		return *GetValuePointer();
	}

	/**
	 Get non-const reference to the value.
	 */
	Type& GetValue() {
		return *GetValuePointer();
	}

	/**
	 Get const pointer to the value.
	 */
	const Type* operator->() const {
		return GetValuePointer();
	}

	/**
	 Get non-const pointer to the value.
	 */
	Type* operator->() {
		return GetValuePointer();
	}

	/**
	 Get const reference to the value.
	 */
	const Type& operator*() const {
		return *GetValuePointer();
	}

	/**
	 Get non-const reference to the value.
 	 */
	Type& operator*() {
		return *GetValuePointer();
	}

	/**
	 Clear the value if exists.
	 */
	void Clear() {
		if (HasValue()) {
			DestroyValue();
		}
	}

private:
	void ConstructValue(const Type& value) {
		new (&value_) Type(value);
		has_value_ = true;
	}

	void DestroyValue() {
		((Type*)&value_)->~Type();
		has_value_ = false;
	}

	const Type* GetValuePointer() const {
		if (has_value_) {
			return reinterpret_cast<const Type*>(&value_);
		}
		return nullptr;
	}

	Type* GetValuePointer() {
		if (has_value_) {
			return reinterpret_cast<Type*>(&value_);
		}
		return nullptr;
	}

private:
	struct {
		unsigned char data[sizeof(Type)];
	} value_;

	bool has_value_;
};

}