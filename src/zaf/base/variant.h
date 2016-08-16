#include <memory>

namespace zaf {

/**
 Represents a value that can store any type's instance.

 The instance stored in Variant must be copyable.
 */
class Variant {
public:
	/**
	 Initialize the instance that has no value.
	 */
	Variant() { }

	/**
	 Initialize the instance that has specified value.
	 */
	template<typename ValueType>
	Variant(const ValueType& value) :
		holder_(std::make_unique<TypedValueHolder<ValueType>>(value)) {

	}

    /**
     Initialize the instance that has specified value.
     */
    template<typename ValueType>
    Variant(ValueType&& value) :
        holder_(std::make_unique<TypedValueHolder<ValueType>>(std::move(value))) {

    }

	/**
	 Initialize the instance that has the same value as another.
	 */
	Variant(const Variant& other) :
		holder_(other.holder_ == nullptr ? nullptr : other.holder_->Clone()) {

	}

	/**
	 Initialize the instance that moves the value from another.
	 */
	Variant(Variant&& other) : holder_(std::move(other.holder_)) { }

	/**
	 Assign the instance with specified value.
	 */
	template<typename ValueType>
	Variant& operator=(const ValueType& value) {
        holder_ = std::make_unique<TypedValueHolder<ValueType>>(value);
		return *this;
	}

    /**
     Assign the instance with specified value.
     */
    template<typename ValueType>
    Variant& operator=(ValueType&& value) {
        holder_ = std::make_unique<TypedValueHolder<ValueType>>(std::move(value));
        return *this;
    }

	/**
	 Assign the instance with another.
	 */
	Variant& operator=(const Variant& other) {
		holder_ = other.holder_ == nullptr ? nullptr : other.holder_->Clone();
		return *this;
	}

    /**
     Assign the instance that obtains value from another.
     */
    Variant& operator=(Variant&& other) {
        holder_ = std::move(other.holder_);
        return *this;
    }

	/**
	 Try to cast the value to specified type.

	 If CastType doesn't match the value's original type,
	 the casting failed, and returns nullptr.
	 */
	template<typename CastType>
	CastType* TryCast() {
		return InnerTryCast<CastType>();
	}

	/**
	 Try to cast the value to specified type.
	 */
	template<typename CastType>
	const CastType* TryCast() const {
		return InnerTryCast<CastType>();
	}

private:
	class ValueHolder {
	public:
		virtual ~ValueHolder() { }

		virtual std::unique_ptr<ValueHolder> Clone() const = 0;
	};

	template<typename ValueType>
	class TypedValueHolder : public ValueHolder {
	public:
		TypedValueHolder(const ValueType& value) : value_(value) { }
        TypedValueHolder(ValueType&& value) : value_(std::move(value)) { }

		std::unique_ptr<ValueHolder> Clone() const {
			return std::make_unique<TypedValueHolder<ValueType>>(value_);
		}

		ValueType* GetValuePointer() {
			return &value_;
		}

	private:
		ValueType value_;
	};

private:
	template<typename CastType>
	CastType* InnerTryCast() const {

		auto typed_value_holder = dynamic_cast<TypedValueHolder<CastType>*>(holder_.get());
		if (typed_value_holder == nullptr) {
			return nullptr;
		}

		return typed_value_holder->GetValuePointer();
	}

private:
	std::unique_ptr<ValueHolder> holder_;
};

}