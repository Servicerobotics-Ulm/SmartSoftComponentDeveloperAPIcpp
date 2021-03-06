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

#ifndef SMARTSOFT_INTERFACES_SMARTITIMERHANDLER_H_
#define SMARTSOFT_INTERFACES_SMARTITIMERHANDLER_H_

#include "smartChronoAliases.h"

namespace Smart {

class ITimerHandler {
public:
	virtual ~ITimerHandler() = default;

	/** hook called on timer expiration
	 *  @param  abs_time   the time-point of the current timeout (which is independent of when the thread actually executed this method)
	 *  @param  act        a pointer (to a pointer) to retrieve the act that was given on
	 *                     scheduleTimer(). Can be used to release resources
	 *                     (see Asynchronous Completion Token (ACT), POSA2).
	 */
	virtual void timerExpired(const TimePoint &abs_time, const void * act) = 0;

	/** hook called when the timer is cancelled
	 *
	 *  Whenever the cancel method of the timer manager is triggered for a timer that is associated with this handler,
	 *  then this method is called. Be aware that this method might be called from within a different thread than
	 *  the timerExpired method, so make sure you do not concurrently write the same data from within the different
	 *  handler methods.
	 */
	virtual void timerCancelled() = 0;

	/** hook called when the timer is deleted
	 *
	 *  Typically when the TimerManager is commanded to shut-down,
	 *  it calls all associated timers to clean-up their resources using this callback.
	 *  Be aware that this method might be called from within a different thread than
	 *  the timerExpired method, so make sure you do not concurrently write the same data
	 *  from within the different handler methods.
	 *
	 *  @param  act        a pointer (to a pointer) to retrieve the act that was given on
	 *                     scheduleTimer(). Can be used to release resources
	 *                     (see Asynchronous Completion Token (ACT), POSA2).
	 */
	virtual void timerDeleted(const void * act) = 0;
};

} /* namespace Smart */

#endif /* SMARTSOFT_INTERFACES_SMARTITIMERHANDLER_H_ */
