/*
 * Copyright (C) 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2012 Digia Plc. and/or its subsidiary(-ies)
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/page/AutoscrollController.h"

#include "core/page/Chrome.h"
#include "core/page/ChromeClient.h"
#include "core/page/EventHandler.h"
#include "core/page/Frame.h"
#include "core/page/FrameView.h"
#include "core/page/Page.h"
#include "core/platform/ScrollView.h"
#include "core/rendering/HitTestResult.h"
#include "core/rendering/RenderBox.h"
#include <wtf/CurrentTime.h>

namespace WebCore {

// Delay time in second for start autoscroll if pointer is in border edge of scrollable element.
static double autoscrollDelay = 0.2;

// When the autoscroll or the panScroll is triggered when do the scroll every 0.05s to make it smooth
static const double autoscrollInterval = 0.05;

#if ENABLE(PAN_SCROLLING)
static Frame* getMainFrame(Frame* frame)
{
    Page* page = frame->page();
    return page ? page->mainFrame() : 0;
}
#endif

AutoscrollController::AutoscrollController(Frame* frame)
    : m_autoscrollTimer(this, &AutoscrollController::autoscrollTimerFired)
    , m_autoscrollRenderer(0)
    , m_autoscrollType(NoAutoscroll)
    , m_dragAndDropAutoscrollStartTime(0)
    , m_frame(frame)
#if ENABLE(PAN_SCROLLING)
    , m_panScrollInProgress(false)
#endif
{
}

RenderBox* AutoscrollController::autoscrollRenderer() const
{
    return m_autoscrollRenderer;
}

bool AutoscrollController::autoscrollInProgress() const
{
    return m_autoscrollType == AutoscrollForSelection;
}

void AutoscrollController::startAutoscrollForSelection(RenderObject* renderer)
{
    // We don't want to trigger the autoscroll or the panScroll if it's already active
    if (m_autoscrollTimer.isActive())
        return;
    RenderBox* scrollable = RenderBox::findAutoscrollable(renderer);
    if (!scrollable)
        return;
    m_autoscrollType = AutoscrollForSelection;
    m_autoscrollRenderer = scrollable;
    startAutoscrollTimer();
}

void AutoscrollController::stopAutoscrollTimer(bool rendererIsBeingDestroyed)
{
#if ENABLE(PAN_SCROLLING)
    didPanScrollStop();
#endif

    RenderBox* scrollable = m_autoscrollRenderer;
    m_autoscrollTimer.stop();
    m_autoscrollRenderer = 0;

    if (!scrollable)
        return;

    EventHandler* eventHandler = m_frame->eventHandler();
    if (autoscrollInProgress() && eventHandler->mouseDownWasInSubframe()) {
        if (Frame* subframe = eventHandler->subframeForTargetNode(eventHandler->mousePressNode()))
            subframe->eventHandler()->stopAutoscrollTimer(rendererIsBeingDestroyed);
        return;
    }

    if (!rendererIsBeingDestroyed)
        scrollable->stopAutoscroll();
#if ENABLE(PAN_SCROLLING)
    if (m_autoscrollType == AutoscrollForPanCanStop || m_autoscrollType == AutoscrollForPan) {
        if (FrameView* view = m_frame->view()) {
            view->removePanScrollIcon();
            view->setCursor(pointerCursor());
        }
    }
#endif

    m_autoscrollType = NoAutoscroll;
}

void AutoscrollController::updateAutoscrollRenderer()
{
    if (!m_autoscrollRenderer)
        return;

    RenderObject* renderer = m_autoscrollRenderer;

#if ENABLE(PAN_SCROLLING)
    HitTestResult hitTest = m_autoscrollRenderer->frame()->eventHandler()->hitTestResultAtPoint(m_panScrollStartPos, true);

    if (Node* nodeAtPoint = hitTest.innerNode())
        renderer = nodeAtPoint->renderer();
#endif

    while (renderer && !(renderer->isBox() && toRenderBox(renderer)->canAutoscroll()))
        renderer = renderer->parent();
    m_autoscrollRenderer = renderer && renderer->isBox() ? toRenderBox(renderer) : 0;
}

void AutoscrollController::updateDragAndDrop(Node* dropTargetNode, const IntPoint& eventPosition, double eventTime)
{
    if (!dropTargetNode) {
        stopAutoscrollTimer();
        return;
    }

    RenderBox* scrollable = RenderBox::findAutoscrollable(dropTargetNode->renderer());
    if (!scrollable) {
        stopAutoscrollTimer();
        return;
    }

    Page* page = m_frame->page();
    if (!page || !page->chrome()->client()->shouldAutoscrollForDragAndDrop(scrollable)) {
        stopAutoscrollTimer();
        return;
    }

    IntSize offset = scrollable->calculateAutoscrollDirection(eventPosition);
    if (offset.isZero()) {
        stopAutoscrollTimer();
        return;
    }

    m_dragAndDropAutoscrollReferencePosition = eventPosition + offset;

    if (m_autoscrollType == NoAutoscroll) {
        m_autoscrollType = AutoscrollForDragAndDrop;
        m_autoscrollRenderer = scrollable;
        m_dragAndDropAutoscrollStartTime = eventTime;
        startAutoscrollTimer();
    } else if (m_autoscrollRenderer != scrollable) {
        m_dragAndDropAutoscrollStartTime = eventTime;
        m_autoscrollRenderer = scrollable;
    }
}

#if ENABLE(PAN_SCROLLING)
// Because we don't capture mouse during pan scrolling, we use main frame's
// AutoScrollController to check whether pan scroll is in progress or not.
void AutoscrollController::didPanScrollStart()
{
    Frame* mainFrame = getMainFrame(m_frame);
    if (mainFrame && mainFrame != m_frame)
        mainFrame->eventHandler()->didPanScrollStart();
    else
        m_panScrollInProgress = true;
}

void AutoscrollController::didPanScrollStop()
{
    Frame* mainFrame = getMainFrame(m_frame);
    if (mainFrame && mainFrame != m_frame)
        mainFrame->eventHandler()->didPanScrollStop();
    else
        m_panScrollInProgress = false;
}

void AutoscrollController::handleMouseReleaseEvent(const PlatformMouseEvent& mouseEvent)
{
    switch (m_autoscrollType) {
    case AutoscrollForPan:
        if (mouseEvent.button() == MiddleButton)
            m_autoscrollType = AutoscrollForPanCanStop;
        break;
    case AutoscrollForPanCanStop:
        stopAutoscrollTimer();
        break;
    }
}

bool AutoscrollController::panScrollInProgress() const
{
    Frame* mainFrame = getMainFrame(m_frame);
    if (mainFrame && mainFrame != m_frame)
        return mainFrame->eventHandler()->panScrollInProgress();
    return m_panScrollInProgress;
}

void AutoscrollController::startPanScrolling(RenderBox* scrollable, const IntPoint& lastKnownMousePosition)
{
    // We don't want to trigger the autoscroll or the panScroll if it's already active
    if (m_autoscrollTimer.isActive())
        return;

    m_autoscrollType = AutoscrollForPan;
    m_autoscrollRenderer = scrollable;
    m_panScrollStartPos = lastKnownMousePosition;

    if (FrameView* view = m_frame->view())
        view->addPanScrollIcon(lastKnownMousePosition);
    didPanScrollStart();
    startAutoscrollTimer();
}
#else
bool AutoscrollController::panScrollInProgress() const
{
    return false;
}
#endif

void AutoscrollController::autoscrollTimerFired(Timer<AutoscrollController>*)
{
    if (!m_autoscrollRenderer) {
        stopAutoscrollTimer();
        return;
    }

    switch (m_autoscrollType) {
    case AutoscrollForDragAndDrop:
        if (WTF::currentTime() - m_dragAndDropAutoscrollStartTime > autoscrollDelay)
            m_autoscrollRenderer->autoscroll(m_dragAndDropAutoscrollReferencePosition);
        break;
    case AutoscrollForSelection: {
        EventHandler* eventHandler = m_frame->eventHandler();
        if (!eventHandler->mousePressed()) {
            stopAutoscrollTimer();
            return;
        }
        eventHandler->updateSelectionForMouseDrag();
        m_autoscrollRenderer->autoscroll(eventHandler->lastKnownMousePosition());
        break;
    }
    case NoAutoscroll:
        break;
#if ENABLE(PAN_SCROLLING)
    case AutoscrollForPanCanStop:
    case AutoscrollForPan:
        if (!panScrollInProgress()) {
            stopAutoscrollTimer();
            return;
        }
        if (FrameView* view = m_frame->view())
            updatePanScrollState(view, m_frame->eventHandler()->lastKnownMousePosition());
        m_autoscrollRenderer->panScroll(m_panScrollStartPos);
        break;
#endif
    }
}

void AutoscrollController::startAutoscrollTimer()
{
    m_autoscrollTimer.startRepeating(autoscrollInterval);
}

#if ENABLE(PAN_SCROLLING)
void AutoscrollController::updatePanScrollState(FrameView* view, const IntPoint& lastKnownMousePosition)
{
    // At the original click location we draw a 4 arrowed icon. Over this icon there won't be any scroll
    // So we don't want to change the cursor over this area
    bool east = m_panScrollStartPos.x() < (lastKnownMousePosition.x() - ScrollView::noPanScrollRadius);
    bool west = m_panScrollStartPos.x() > (lastKnownMousePosition.x() + ScrollView::noPanScrollRadius);
    bool north = m_panScrollStartPos.y() > (lastKnownMousePosition.y() + ScrollView::noPanScrollRadius);
    bool south = m_panScrollStartPos.y() < (lastKnownMousePosition.y() - ScrollView::noPanScrollRadius);

    if (m_autoscrollType == AutoscrollForPan && (east || west || north || south))
        m_autoscrollType = AutoscrollForPanCanStop;

    if (north) {
        if (east)
            view->setCursor(northEastPanningCursor());
        else if (west)
            view->setCursor(northWestPanningCursor());
        else
            view->setCursor(northPanningCursor());
    } else if (south) {
        if (east)
            view->setCursor(southEastPanningCursor());
        else if (west)
            view->setCursor(southWestPanningCursor());
        else
            view->setCursor(southPanningCursor());
    } else if (east)
        view->setCursor(eastPanningCursor());
    else if (west)
        view->setCursor(westPanningCursor());
    else
        view->setCursor(middlePanningCursor());
}
#endif

} // namespace WebCore