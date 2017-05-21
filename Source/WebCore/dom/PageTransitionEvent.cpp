/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "PageTransitionEvent.h"

#include "EventNames.h"

namespace WebCore {

PageTransitionEventInit::PageTransitionEventInit()
    : persisted(false)
{
}

PageTransitionEvent::PageTransitionEvent()
    : m_persisted(false)
{
}

PageTransitionEvent::PageTransitionEvent(const AtomicString& type, bool persisted)
    : Event(type, true, true)
    , m_persisted(persisted)
{
}

PageTransitionEvent::PageTransitionEvent(const AtomicString& type, const PageTransitionEventInit& initializer)
    : Event(type, initializer)
    , m_persisted(initializer.persisted)
{
}

PageTransitionEvent::~PageTransitionEvent()
{
}

void PageTransitionEvent::initPageTransitionEvent(const AtomicString& type, 
                                            bool canBubbleArg,
                                            bool cancelableArg,
                                            bool persisted)
{
    if (dispatched())
        return;

    initEvent(type, canBubbleArg, cancelableArg);

    m_persisted = persisted;
}

const AtomicString& PageTransitionEvent::interfaceName() const
{
    return eventNames().interfaceForPageTransitionEvent;
}

} // namespace WebCore
