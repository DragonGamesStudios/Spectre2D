#include "..\include\Spectre2D\LockableObject.h"

namespace sp
{
	LockableObject::LockableObject(bool lock)
	{
		locked = lock;
	}

	void LockableObject::setLocked(bool value)
	{
		locked = value;
	}

	bool LockableObject::getLocked() const
	{
		return locked;
	}

}