#pragma once

namespace Oak
{
	/**
	\ingroup gr_code_common
	*/

	class CLASS_DECLSPEC Object
	{
	public:
		typedef void (Object::*Delegate)(float timedelta);
		typedef void (Object::*DelegateSimple)();

		virtual ~Object() {};
		virtual void Release() {};
	};

	struct CLASS_DECLSPEC DelegateObject
	{
		Object* entity;
		Object::Delegate call;
	};
}