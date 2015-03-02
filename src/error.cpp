#include "milk/error.h"
#include <sstream>
#include <iomanip>
using namespace milk;
using namespace std;

CLog *error::milk::log=0;
Level error::milk::level;

#if MILK_EH_TRANSLATOR
void milk::my_translator(unsigned code, EXCEPTION_POINTERS *info)
{
	ostringstream oss;
	oss << hex << "0x" << uppercase << code;
	switch (code)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		oss<<": Access violation at ";
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		oss<<": Array bounds exceeded at ";
		break;
	case EXCEPTION_BREAKPOINT:
		oss<<": Breakpoint at ";
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		oss<<": Misalignment at ";
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		oss<<": Denormal float operand at ";
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		oss<<": Float division by zero at ";
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		oss<<": Inexact float result at ";
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		oss<<": Invalid float operation at ";
		break;
	case EXCEPTION_FLT_OVERFLOW:
		oss<<": Float overflow at ";
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		oss<<": Float stack over- or underflow at ";
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		oss<<": Float underflow at ";
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		oss<<": Illegal instruction at ";
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		oss<<": Page error at ";
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		oss<<": Integer division by zero at ";
		break;
	case EXCEPTION_INT_OVERFLOW:
		oss<<": Integer overflow at ";
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		oss<<": Invalid disposition at ";
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		oss<<": Noncontinuable exception at ";
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		oss<<": Instruction not allowed at ";
		break;
	case EXCEPTION_SINGLE_STEP:
		oss<<": Single step at ";
		break;
	case EXCEPTION_STACK_OVERFLOW:
		oss<<": Stack overflow at ";
		break;
	default:
		oss<<": Unknown SEH exception at ";
		break;
	}
	oss << "0x" << info->ExceptionRecord->ExceptionAddress;
	throw error::milk(oss.str());
}
#endif

