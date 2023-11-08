/* soapServer.cpp
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

SOAP_SOURCE_STAMP("@(#) soapServer.cpp ver 2.8.130 2023-11-07 15:33:00 GMT")
extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve(struct soap *soap)
{
#ifndef WITH_FASTCGI
	soap->keep_alive = soap->max_keep_alive + 1;
#endif
	do
	{
#ifndef WITH_FASTCGI
		if (soap->keep_alive > 0 && soap->max_keep_alive > 0)
			soap->keep_alive--;
#endif
		if (soap_begin_serve(soap))
		{	if (soap->error >= SOAP_STOP)
				continue;
			return soap->error;
		}
		if ((soap_serve_request(soap) || (soap->fserveloop && soap->fserveloop(soap))) && soap->error && soap->error < SOAP_STOP)
		{
#ifdef WITH_FASTCGI
			soap_send_fault(soap);
#else
			return soap_send_fault(soap);
#endif
		}
#ifdef WITH_FASTCGI
		soap_destroy(soap);
		soap_end(soap);
	} while (1);
#else
	} while (soap->keep_alive);
#endif
	return SOAP_OK;
}

#ifndef WITH_NOSERVEREQUEST
extern "C" SOAP_FMAC5 int SOAP_FMAC6 soap_serve_request(struct soap *soap)
{
	(void)soap_peek_element(soap);
	if (!soap_match_tag(soap, soap->tag, "calcns:add"))
		return soap_serve_calcns__add(soap);
	if (!soap_match_tag(soap, soap->tag, "calcns:subs"))
		return soap_serve_calcns__subs(soap);
	if (!soap_match_tag(soap, soap->tag, "calcns:mult"))
		return soap_serve_calcns__mult(soap);
	if (!soap_match_tag(soap, soap->tag, "calcns:div"))
		return soap_serve_calcns__div(soap);
	return soap->error = SOAP_NO_METHOD;
}
#endif

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_calcns__add(struct soap *soap)
{	struct calcns__add soap_tmp_calcns__add;
	struct calcns__addResponse soap_tmp_calcns__addResponse;
	int soap_tmp_int;
	soap_default_calcns__addResponse(soap, &soap_tmp_calcns__addResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_calcns__addResponse.res = &soap_tmp_int;
	soap_default_calcns__add(soap, &soap_tmp_calcns__add);
	if (!soap_get_calcns__add(soap, &soap_tmp_calcns__add, "calcns:add", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = calcns__add(soap, soap_tmp_calcns__add.a, soap_tmp_calcns__add.b, soap_tmp_calcns__addResponse.res);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = ""; /* use SOAP encoding style */
	soap_serializeheader(soap);
	soap_serialize_calcns__addResponse(soap, &soap_tmp_calcns__addResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_calcns__addResponse(soap, &soap_tmp_calcns__addResponse, "calcns:addResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_calcns__addResponse(soap, &soap_tmp_calcns__addResponse, "calcns:addResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_calcns__subs(struct soap *soap)
{	struct calcns__subs soap_tmp_calcns__subs;
	struct calcns__subsResponse soap_tmp_calcns__subsResponse;
	int soap_tmp_int;
	soap_default_calcns__subsResponse(soap, &soap_tmp_calcns__subsResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_calcns__subsResponse.res = &soap_tmp_int;
	soap_default_calcns__subs(soap, &soap_tmp_calcns__subs);
	if (!soap_get_calcns__subs(soap, &soap_tmp_calcns__subs, "calcns:subs", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = calcns__subs(soap, soap_tmp_calcns__subs.a, soap_tmp_calcns__subs.b, soap_tmp_calcns__subsResponse.res);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = ""; /* use SOAP encoding style */
	soap_serializeheader(soap);
	soap_serialize_calcns__subsResponse(soap, &soap_tmp_calcns__subsResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_calcns__subsResponse(soap, &soap_tmp_calcns__subsResponse, "calcns:subsResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_calcns__subsResponse(soap, &soap_tmp_calcns__subsResponse, "calcns:subsResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_calcns__mult(struct soap *soap)
{	struct calcns__mult soap_tmp_calcns__mult;
	struct calcns__multResponse soap_tmp_calcns__multResponse;
	int soap_tmp_int;
	soap_default_calcns__multResponse(soap, &soap_tmp_calcns__multResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_calcns__multResponse.res = &soap_tmp_int;
	soap_default_calcns__mult(soap, &soap_tmp_calcns__mult);
	if (!soap_get_calcns__mult(soap, &soap_tmp_calcns__mult, "calcns:mult", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = calcns__mult(soap, soap_tmp_calcns__mult.a, soap_tmp_calcns__mult.b, soap_tmp_calcns__multResponse.res);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = ""; /* use SOAP encoding style */
	soap_serializeheader(soap);
	soap_serialize_calcns__multResponse(soap, &soap_tmp_calcns__multResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_calcns__multResponse(soap, &soap_tmp_calcns__multResponse, "calcns:multResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_calcns__multResponse(soap, &soap_tmp_calcns__multResponse, "calcns:multResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

SOAP_FMAC5 int SOAP_FMAC6 soap_serve_calcns__div(struct soap *soap)
{	struct calcns__div soap_tmp_calcns__div;
	struct calcns__divResponse soap_tmp_calcns__divResponse;
	int soap_tmp_int;
	soap_default_calcns__divResponse(soap, &soap_tmp_calcns__divResponse);
	soap_default_int(soap, &soap_tmp_int);
	soap_tmp_calcns__divResponse.res = &soap_tmp_int;
	soap_default_calcns__div(soap, &soap_tmp_calcns__div);
	if (!soap_get_calcns__div(soap, &soap_tmp_calcns__div, "calcns:div", NULL))
		return soap->error;
	if (soap_body_end_in(soap)
	 || soap_envelope_end_in(soap)
	 || soap_end_recv(soap))
		return soap->error;
	soap->error = calcns__div(soap, soap_tmp_calcns__div.a, soap_tmp_calcns__div.b, soap_tmp_calcns__divResponse.res);
	if (soap->error)
		return soap->error;
	soap->encodingStyle = ""; /* use SOAP encoding style */
	soap_serializeheader(soap);
	soap_serialize_calcns__divResponse(soap, &soap_tmp_calcns__divResponse);
	if (soap_begin_count(soap))
		return soap->error;
	if ((soap->mode & SOAP_IO_LENGTH))
	{	if (soap_envelope_begin_out(soap)
		 || soap_putheader(soap)
		 || soap_body_begin_out(soap)
		 || soap_put_calcns__divResponse(soap, &soap_tmp_calcns__divResponse, "calcns:divResponse", "")
		 || soap_body_end_out(soap)
		 || soap_envelope_end_out(soap))
			 return soap->error;
	};
	if (soap_end_count(soap)
	 || soap_response(soap, SOAP_OK)
	 || soap_envelope_begin_out(soap)
	 || soap_putheader(soap)
	 || soap_body_begin_out(soap)
	 || soap_put_calcns__divResponse(soap, &soap_tmp_calcns__divResponse, "calcns:divResponse", "")
	 || soap_body_end_out(soap)
	 || soap_envelope_end_out(soap)
	 || soap_end_send(soap))
		return soap->error;
	return soap_closesock(soap);
}

#if defined(__BORLANDC__)
#pragma option pop
#pragma option pop
#endif

/* End of soapServer.cpp */