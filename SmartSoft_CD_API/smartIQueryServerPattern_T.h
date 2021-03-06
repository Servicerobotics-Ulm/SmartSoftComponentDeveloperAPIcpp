//===================================================================================
//
//  Copyright (C) 2017 Alex Lotz, Dennis Stampfer, Matthias Lutz, Christian Schlegel
//
//        lotz@hs-ulm.de
//        stampfer@hs-ulm.de
//        lutz@hs-ulm.de
//        schlegel@hs-ulm.de
//
//        Servicerobotik Ulm
//        Christian Schlegel
//        Ulm University of Applied Sciences
//        Prittwitzstr. 10
//        89075 Ulm
//        Germany
//
//  This file is part of the SmartSoft Component-Developer C++ API.
//
//  Redistribution and use in source and binary forms, with or without modification,
//  are permitted provided that the following conditions are met:
//
//  1. Redistributions of source code must retain the above copyright notice,
//     this list of conditions and the following disclaimer.
//
//  2. Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation
//     and/or other materials provided with the distribution.
//
//  3. Neither the name of the copyright holder nor the names of its contributors
//     may be used to endorse or promote products derived from this software
//     without specific prior written permission.
//
//  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
//  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
//  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
//  INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
//  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
//  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
//  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
//  OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
//  OF THE POSSIBILITY OF SUCH DAMAGE.
//
//===================================================================================

#ifndef SMARTSOFT_INTERFACES_SMARTIQUERYSERVERPATTERN_T_H_
#define SMARTSOFT_INTERFACES_SMARTIQUERYSERVERPATTERN_T_H_

#include <memory>

#include "smartIInputHandler_T.h"
#include "smartIServerPattern.h"
#include "smartQueryStatus.h"
#include "smartICorrelationId.h"

namespace Smart {

// this is the middleware-independent shared-pointer Alias (hides internal, middleware-specific ID implementation)
using QueryIdPtr = CorrelationIdPtr;

// forward declaration
template<class RequestType, class AnswerType>
class IQueryServerPattern;

/** Handler Class for QueryServer for incoming requests.
 *
 *  Used by the QueryServer to handle incoming queries.
 *  The user should implement the handleQuery() method by
 *  subclassing and providing a pointer to an IQueryServerPattern
 *  to this handler.
 */
template<class RequestType, class AnswerType>
class IQueryServerHandler
{
public:

	/** Default destructor
	 */
	virtual ~IQueryServerHandler() = default;

	// these aliases can be used in derived classes to simplify creation of the constructors
	using IQueryServer = IQueryServerPattern<RequestType,AnswerType>;

	/** Handler method for an incoming query request.
	 *
	 *  This method is called by the query-server every time
	 *  a new query request is received. It must be provided by the
	 *  component developer to handle incoming requests. Since the
	 *  method is executed by the communication thread, it must be
	 *  very fast and non-blocking. Within this handler, use the
	 *  provided <b>server</b> pointer to provide an answer like
	 *  this: <b>"server->answer(...)"</b>.
	 *
	 *  Usually the request and the id will be inserted into a queue
	 *  and another working thread processes the request and provides
	 *  the result. The ThreadedQueryHandler decorator provides such
	 *  a processing pattern.
	 *
	 *  @param server   a reference to the related query server pattern for calling the answer(...) method from within the handler
	 *  @param id       id of new query
	 *  @param request  the request itself
	 */
	virtual void handleQuery(IQueryServer &server, const QueryIdPtr &id, const RequestType& request) = 0;
};


/** The server part of the Query pattern to perform two-way (request-response) communication.
 *
 *  Template parameters
 *    - <b>RequestType</b>: request class (Communication Object)
 *    - <b>AnswerType</b>: answer (reply) class (Communication Object)
 */
template<class RequestType, class AnswerType>
class IQueryServerPattern
:	public IServerPattern
,	public InputSubject<std::pair<QueryIdPtr,RequestType>>
{
private:
	std::shared_ptr<IQueryServerHandler<RequestType,AnswerType>> query_handler;
protected:
	/// please call this method in derived classes from within the middleware-specific data handler
	void handleQuery(const QueryIdPtr id, const RequestType& request) {
		//TODO: this copy should not be necessary, however, there seems to be a bug somewhere
		// that corrupts the request object within the handleQuery call so the follow-up
		// notify_input call causes a segmentation fault, the local copy is just
		// a workaround and should be fixed
		std::pair<QueryIdPtr,RequestType> local_copy(id,request);
		if(query_handler) {
			query_handler->handleQuery(*this, id, request);
		}
		this->notify_input(local_copy);
	}
public:
	// be aware of nondependent types when using this alias in derived classes
	// see: https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-types
	using IQueryServerHandlerPtr = std::shared_ptr<Smart::IQueryServerHandler<RequestType,AnswerType>>;

    /** Default constructor.
     *
     *  Note that a handler has to be supplied. Without a handler, the
     *  QueryServer could not accept a query.
     *
     *  @param component     management class of the component
     *  @param service       name of the service
     *  @param query_handler an optional query_handler
     */
    IQueryServerPattern(IComponent* component, const std::string& service, IQueryServerHandlerPtr query_handler = nullptr)
    :   IServerPattern(component, service)
    ,   query_handler(query_handler)
    {  }

    /** Destructor.
     *  Properly disconnects all service requestors in case of destruction
     *  such that all pending queries are handled correctly at client side
     *  even when the service provider disappears during pending queries.
     */
	virtual ~IQueryServerPattern() = default;

    /** Provide answer to be sent back to the requestor.
     *
     *  Member function is thread safe and thread reentrant.
     *
     *  @param id identifies the request to which the answer belongs
     *  @param answer is the reply itself.
     *
     *  @return status code:
     *    - SMART_OK                  : everything is ok and answer sent to requesting client
     *    - SMART_WRONGID             : no pending query with that <I>id</I> known
     *    - SMART_DISCONNECTED        : answer not needed anymore since client
     *                                  got disconnected meanwhile
     *    - SMART_ERROR_COMMUNICATION : communication problems
     *    - SMART_ERROR               : something went wrong
     */
    virtual StatusCode answer(const QueryIdPtr id, const AnswerType& answer) = 0;
};

} /* namespace Smart */

#endif /* SMARTSOFT_INTERFACES_SMARTIQUERYSERVERPATTERN_T_H_ */
