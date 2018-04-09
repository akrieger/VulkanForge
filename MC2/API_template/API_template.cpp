// API_template.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "API_template.h"


// This is an example of an exported variable
API_TEMPLATE_API int nAPI_template=0;

// This is an example of an exported function.
API_TEMPLATE_API int fnAPI_template(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see API_template.h for the class definition
CAPI_template::CAPI_template()
{
	return;
}
