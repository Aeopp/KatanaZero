#pragma once
#include <memory>
#include <mutex>
template<typename SubType>
class singleton_interface abstract
{
private:
	singleton_interface() = default;
protected:
	virtual ~singleton_interface()noexcept = default;
public:
	singleton_interface(singleton_interface&&) noexcept= delete;
	singleton_interface(const singleton_interface&)= delete;
	singleton_interface& operator=(singleton_interface&&)noexcept = delete;
	singleton_interface& operator=(const singleton_interface&)= delete;
public:
	friend typename  SubType;
	friend class std::shared_ptr<singleton_interface>;
public:
	static SubType& instance();
};

template<typename SubType>
inline SubType& singleton_interface<SubType>::instance()
{
	static std::once_flag _flag;

	static std::shared_ptr<SubType> _instance{ nullptr };

	static auto make_instance =
		[](auto& _instance_ptr) {
		_instance_ptr = std::make_shared<SubType>();
	};

	std::call_once(_flag, make_instance, _instance);

	return *_instance.get();
}

