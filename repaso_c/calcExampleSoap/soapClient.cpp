/* soapClient.cpp
   Generated by gSOAP 2.8.130 for calc.h

gSOAP XML Web services tools
Copyright (C) 2000-2023, Robert van Engelen, Genivia Inc. All Rights Reserved.
The soapcpp2 tool and its generated software are released under the GPL.
This program is released under the GPL with the additional exemption that
compiling, linking, and/or using OpenSSL is allowed.
--------------------------------------------------------------------------------
A commercial use license is available from Genivia Inc., contact@genivia.com
--------------------------------------------------------------------------------
*/

#if defined(__BORLANDC__)
#pragma option push -w-8060
#pragma option push -w-8004
#endif
#include "soapH.h"

SOAP_SOURCE_STAMP("@(#) soapClient.cpp ver 2.8.130 2023-11-07 15:33:00 GMT")


SOAP_FMAC5 int SOAP_FMAC6 soap_call_calcns__add(struct soap *soap, const char *soap_endpoint, const char *soap_action, int a, int b, int *res)
{	if (soap_send_calcns__add(soap, soap_endpoint, soap_action, a, b) || soap_recv_calcns__add(soap, res))
		return soap->error;
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_send_calcns__add(struct soap *soap, const char *soap_endpoint, const char *soap_action, int a, int b)
{	struct calcns__add soap_tmp_calcns__add;
	if (soap_endpoint == NULL)
		soap_endpoint = "http//localhost:10000";
	soap_tmp_calcns__add.a = a;
	soap_tmp_calcns__add.b = b;
	soap_begin(soap);
	soap->encodingStyle = ""; /* use SOAP encoding style */
	soap_serializeheader(soap);
	soap_serialize_calcns__add(soap, &soap_tmp_calcns__add);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_calcns__add(soap, &soap_tmp_calcns__add, "calcns:add", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_calcns__add(soap, &soap_tmp_calcns__add, "calcns:add", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_recv_calcns__add(struct soap *soap, int *res)
{
	struct calcns__addResponse *soap_tmp_calcns__addResponse;
	if (!res)
		return soap_closesock(soap);
	soap_default_int(soap, res);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	if (soap_recv_fault(soap, 1))
		return soap->error;
	soap_tmp_calcns__addResponse = soap_get_calcns__addResponse(soap, NULL, "", NULL);
	if (!soap_tmp_calcns__addResponse || soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	if (res && soap_tmp_calcns__addResponse->res)
		*res = *soap_tmp_calcns__addResponse->res;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_calcns__subs(struct soap *soap, const char *soap_endpoint, const char *soap_action, int a, int b, int *res)
{	if (soap_send_calcns__subs(soap, soap_endpoint, soap_action, a, b) || soap_recv_calcns__subs(soap, res))
		return soap->error;
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_send_calcns__subs(struct soap *soap, const char *soap_endpoint, const char *soap_action, int a, int b)
{	struct calcns__subs soap_tmp_calcns__subs;
	if (soap_endpoint == NULL)
		soap_endpoint = "http//localhost:10000";
	soap_tmp_calcns__subs.a = a;
	soap_tmp_calcns__subs.b = b;
	soap_begin(soap);
	soap->encodingStyle = ""; /* use SOAP encoding style */
	soap_serializeheader(soap);
	soap_serialize_calcns__subs(soap, &soap_tmp_calcns__subs);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_calcns__subs(soap, &soap_tmp_calcns__subs, "calcns:subs", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_calcns__subs(soap, &soap_tmp_calcns__subs, "calcns:subs", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_recv_calcns__subs(struct soap *soap, int *res)
{
	struct calcns__subsResponse *soap_tmp_calcns__subsResponse;
	if (!res)
		return soap_closesock(soap);
	soap_default_int(soap, res);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	if (soap_recv_fault(soap, 1))
		return soap->error;
	soap_tmp_calcns__subsResponse = soap_get_calcns__subsResponse(soap, NULL, "", NULL);
	if (!soap_tmp_calcns__subsResponse || soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	if (res && soap_tmp_calcns__subsResponse->res)
		*res = *soap_tmp_calcns__subsResponse->res;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_calcns__mult(struct soap *soap, const char *soap_endpoint, const char *soap_action, int a, int b, int *res)
{	if (soap_send_calcns__mult(soap, soap_endpoint, soap_action, a, b) || soap_recv_calcns__mult(soap, res))
		return soap->error;
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_send_calcns__mult(struct soap *soap, const char *soap_endpoint, const char *soap_action, int a, int b)
{	struct calcns__mult soap_tmp_calcns__mult;
	if (soap_endpoint == NULL)
		soap_endpoint = "http//localhost:10000";
	soap_tmp_calcns__mult.a = a;
	soap_tmp_calcns__mult.b = b;
	soap_begin(soap);
	soap->encodingStyle = ""; /* use SOAP encoding style */
	soap_serializeheader(soap);
	soap_serialize_calcns__mult(soap, &soap_tmp_calcns__mult);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_calcns__mult(soap, &soap_tmp_calcns__mult, "calcns:mult", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_calcns__mult(soap, &soap_tmp_calcns__mult, "calcns:mult", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_recv_calcns__mult(struct soap *soap, int *res)
{
	struct calcns__multResponse *soap_tmp_calcns__multResponse;
	if (!res)
		return soap_closesock(soap);
	soap_default_int(soap, res);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	if (soap_recv_fault(soap, 1))
		return soap->error;
	soap_tmp_calcns__multResponse = soap_get_calcns__multResponse(soap, NULL, "", NULL);
	if (!soap_tmp_calcns__multResponse || soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	if (res && soap_tmp_calcns__multResponse->res)
		*res = *soap_tmp_calcns__multResponse->res;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_call_calcns__div(struct soap *soap, const char *soap_endpoint, const char *soap_action, int a, int b, int *res)
{	if (soap_send_calcns__div(soap, soap_endpoint, soap_action, a, b) || soap_recv_calcns__div(soap, res))
		return soap->error;
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_send_calcns__div(struct soap *soap, const char *soap_endpoint, const char *soap_action, int a, int b)
{	struct calcns__div soap_tmp_calcns__div;
	if (soap_endpoint == NULL)
		soap_endpoint = "http//localhost:10000";
	soap_tmp_calcns__div.a = a;
	soap_tmp_calcns__div.b = b;
	soap_begin(soap);
	soap->encodingStyle = ""; /* use SOAP encoding style */
	soap_serializeheader(soap);
	soap_serialize_calcns__div(soap, &soap_tmp_calcns__div);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_calcns__div(soap, &soap_tmp_calcns__div, "calcns:div", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	}
	if (soap_end_count(soap))
		return soap->error;
	if (soap_connect(soap, soap_endpoint, soap_action)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_calcns__div(soap, &soap_tmp_calcns__div, "calcns:div", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap_closesock(soap);
	return SOAP_OK;
}

SOAP_FMAC5 int SOAP_FMAC6 soap_recv_calcns__div(struct soap *soap, int *res)
{
	struct calcns__divResponse *soap_tmp_calcns__divResponse;
	if (!res)
		return soap_closesock(soap);
	soap_default_int(soap, res);
	if (soap_begin_recv(soap)
	 || soap_envelope_begin_in(soap)
	 || soap_recv_header(soap)
	 || soap_body_begin_in(soap))
		return soap_closesock(soap);
	if (soap_recv_fault(soap, 1))
		return soap->error;
	soap_tmp_calcns__divResponse = soap_get_calcns__divResponse(soap, NULL, "", NULL);
	if (!soap_tmp_calcns__divResponse || soap->error)
		return soap_recv_fault(soap, 0);
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap_closesock(soap);
	if (res && soap_tmp_calcns__divResponse->res)
		*res = *soap_tmp_calcns__divResponse->res;
	return soap_closesock(soap);
}

#if defined(__BORLANDC__)
#pragma option pop
#pragma option pop
#endif

/* End of soapClient.cpp */
