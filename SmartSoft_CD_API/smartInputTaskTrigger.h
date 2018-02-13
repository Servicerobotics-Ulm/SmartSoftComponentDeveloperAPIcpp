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

#ifndef SMARTSOFT_INTERFACES_SMARTINPUTTASKTRIGGER_H_
#define SMARTSOFT_INTERFACES_SMARTINPUTTASKTRIGGER_H_

#include "smartIInputHandler_T.h"
#include "smartTaskTriggerObserver.h"


namespace Smart {

template <class InputType>
class InputTaskTrigger
:	public IInputHandler<InputType>
,	public TaskTriggerSubject
{
private:
	Smart::StatusCode updateStatus;
	InputType lastUpdate;

	/** This is the main input-handler method that will be automatically called from the given subject
	 *  each time the subject receives input-data.
	 *
	 *  @param input the input-data reference
	 */
	virtual void handle_input(const InputType& input) {
		// store a copy of the input object (used by getUpdate method)
		this->lastUpdate = input;
		this->updateStatus = Smart::SMART_OK;
		// inform all associated tasks about a new update
		this->trigger_all_tasks();
		// delegate handling of the update to a method overloaded in derived classes
		this->on_update(input);
	}
protected:

	/** This method can be overloaded in derived classes for implementing a passive upcall handler
	 *
	 *  @param input the input-data reference
	 */
	virtual void handle_update(const InputType& input) { }

public:
	InputTaskTrigger(InputSubject<InputType> *subject)
	:	IInputHandler<InputType>(subject)
	{
		updateStatus = SMART_NODATA;
	}
	virtual ~InputTaskTrigger()
	{ }

	inline Smart::StatusCode getUpdate(InputType &update) {
		// get a copy of the last update
		update = lastUpdate;
		return updateStatus;
	}
};

} /* namespace Smart */

#endif /* SMARTSOFT_INTERFACES_SMARTINPUTTASKTRIGGER_H_ */
