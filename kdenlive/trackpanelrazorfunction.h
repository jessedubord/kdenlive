/***************************************************************************
                          trackpanelfunction.h  -  description
                             -------------------
    begin                : Sun May 18 2003
    copyright            : (C) 2003 by Jason Wood
    email                : jasonwood@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifndef TRACKPANELRAZORFUNCTION_H
#define TRACKPANELRAZORFUNCTION_H

#include "kdenlive.h"
#include "trackpanelfunction.h"

class KdenliveDoc;
class DocTrackBase;

namespace Gui {
    class KdenliveApp;
    class KTimeLine;
}
/**
Abstract Base Class for track panel functionality decorators. This and it's
derived classes allow different behaviours to be added to panels as required.

@author Jason Wood
*/ class TrackPanelRazorFunction:public TrackPanelFunction
{
  Q_OBJECT public:
    TrackPanelRazorFunction(Gui::KdenliveApp * app,
	Gui::KTimeLine * timeline, KdenliveDoc * document);

    virtual ~ TrackPanelRazorFunction();

	/**
	Returns true if the specified position should cause this function to activate,
	otherwise returns false.
	*/
    virtual bool mouseApplies(Gui::KTrackPanel * panel,
	QMouseEvent * event) const;

	/**
	Returns a relevant mouse cursor for the given mouse position
	*/
    virtual QCursor getMouseCursor(Gui::KTrackPanel * panel,
	QMouseEvent * event);

	/**
	A mouse button has been pressed. Returns true if we want to handle this event
	*/
    virtual bool mousePressed(Gui::KTrackPanel * panel,
	QMouseEvent * event);

	/** Processes Mouse double click.*/
    virtual bool mouseDoubleClicked(Gui::KTrackPanel *, QMouseEvent *);

	/**
	Mouse Release Events in the track view area. Returns true if we have finished
	an operation now.
	*/
    virtual bool mouseReleased(Gui::KTrackPanel *, QMouseEvent * event);

	/**
	Processes Mouse Move events in the track view area. Returns true if we are
	continuing with the drag.*/
    virtual bool mouseMoved(Gui::KTrackPanel *, QMouseEvent *);
    
  signals:			// Signals
	/**
	emitted when a tool is "looking" at a clip, it signifies to whatever is listening
	that displaying this information in some way would be useful.
	*/
        void lookingAtClip(DocClipRef *, const GenTime &);
        void sceneListChanged(bool);
  private:

     Gui::KdenliveApp * m_app;
     Gui::KTimeLine * m_timeline;
     KdenliveDoc *m_document;
     DocClipRef *m_clipUnderMouse;
    QCursor m_razorCursor;
};

#endif
