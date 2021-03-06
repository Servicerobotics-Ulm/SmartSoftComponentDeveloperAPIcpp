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


#ifndef SMARTQUERYSERVERTASKTRIGGER_T_H_
#define SMARTQUERYSERVERTASKTRIGGER_T_H_

#include <list>
#include <mutex>

#include "smartIQueryServerPattern_T.h"
#include "smartTaskTriggerObserver.h"

namespace Smart {

template<class RequestType, class AnswerType>
class QueryServerTaskTrigger
:	public IInputHandler<std::pair<Smart::QueryIdPtr,RequestType>>
,	public TaskTriggerSubject
{
private:
	std::mutex requestMutex;
	using InputType = std::pair<Smart::QueryIdPtr,RequestType>;
	std::list<InputType> requestList;
protected:
	virtual void handle_input(const InputType& input) override {
		std::unique_lock<std::mutex> lock (requestMutex);
		// store the request entry in a list
		requestList.push_back(input);
		// trigger all observer tasks
		this->trigger_all_tasks();
	}
public:
	QueryServerTaskTrigger(IQueryServerPattern<RequestType,AnswerType>* server)
	:	IInputHandler<std::pair<Smart::QueryIdPtr,RequestType>>(server)
	{ }
	virtual ~QueryServerTaskTrigger() = default;

	inline Smart::StatusCode consumeRequest(Smart::QueryIdPtr& id, RequestType &request) {
		std::unique_lock<std::mutex> lock (requestMutex);
		if(!requestList.empty()) {
			// copy request data
			id = requestList.front().first;
			request = requestList.front().second;
			// consume the current request item
			requestList.pop_front();
			return SMART_OK;
		}
		return SMART_NODATA;
	}

	inline Smart::StatusCode answer(const Smart::QueryIdPtr& id, const AnswerType& answer) {
		return this->server->answer(id, answer);
	}
};

} /* namespace Smart */

#endif /* SMARTQUERYSERVERTASKTRIGGER_T_H_ */
