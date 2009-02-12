/***************************************************************************
 *   Copyright (C) 2008 by Jean-Baptiste Mardelle (jb@kdenlive.org)        *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA          *
 ***************************************************************************/


#include <KDebug>

#include "clipitem.h"
#include "clipdurationdialog.h"
#include "kdenlivesettings.h"
#include <QWheelEvent>

ClipDurationDialog::ClipDurationDialog(AbstractClipItem *clip, Timecode tc, QWidget * parent): QDialog(parent), m_tc(tc), m_clip(clip) {
    setFont(KGlobalSettings::toolBarFont());
    m_fps = m_tc.fps();
    m_view.setupUi(this);

    bool allowCrop = true;
    if (clip->type() == AVWIDGET) {
        ClipItem *item = static_cast <ClipItem *>(clip);
        int t = item->clipType();
        if (t == COLOR || t == IMAGE || t == TEXT) allowCrop = false;
    }

    if (!allowCrop || clip->type() == TRANSITIONWIDGET) {
        m_view.crop_up->hide();
        m_view.crop_down->hide();
        m_view.crop_position->hide();
        m_view.crop_label->hide();
    }

    m_view.clip_position->setText(tc.getTimecode(m_clip->startPos(), m_fps));
    m_view.crop_position->setText(tc.getTimecode(m_clip->cropStart(), m_fps));
    m_view.clip_duration->setText(tc.getTimecode(m_clip->duration(), m_fps));
    connect(m_view.position_up, SIGNAL(clicked()), this, SLOT(slotPosUp()));
    connect(m_view.position_down, SIGNAL(clicked()), this, SLOT(slotPosDown()));
    connect(m_view.crop_up, SIGNAL(clicked()), this, SLOT(slotCropUp()));
    connect(m_view.crop_down, SIGNAL(clicked()), this, SLOT(slotCropDown()));
    connect(m_view.duration_up, SIGNAL(clicked()), this, SLOT(slotDurUp()));
    connect(m_view.duration_down, SIGNAL(clicked()), this, SLOT(slotDurDown()));
    connect(m_view.crop_position, SIGNAL(textChanged(const QString &)), this, SLOT(slotCheckCrop()));
    adjustSize();
}

ClipDurationDialog::~ClipDurationDialog() {
}

void ClipDurationDialog::setMargins(GenTime min, GenTime max) {
    m_min = min;
    m_max = max;
    connect(m_view.clip_position, SIGNAL(textChanged(const QString &)), this, SLOT(slotCheckStart()));
    connect(m_view.clip_duration, SIGNAL(textChanged(const QString &)), this, SLOT(slotCheckDuration()));
}

void ClipDurationDialog::slotCheckStart() {
    int pos = m_tc.getFrameCount(m_view.clip_position->text(), m_fps);
    int dur = m_tc.getFrameCount(m_view.clip_duration->text(), m_fps);
    GenTime start(pos, m_fps);
    GenTime duration(dur, m_fps);
    if (start < m_min) {
        m_view.clip_position->setText(m_tc.getTimecode(m_min, m_fps));
    } else if (start + duration > m_max) {
        m_view.clip_position->setText(m_tc.getTimecode(m_max - duration, m_fps));
    }
}

void ClipDurationDialog::slotCheckDuration() {
    int pos = m_tc.getFrameCount(m_view.clip_position->text(), m_fps);
    int dur = m_tc.getFrameCount(m_view.clip_duration->text(), m_fps);
    GenTime start(pos, m_fps);
    GenTime duration(dur, m_fps);
    GenTime maxDuration = m_max == GenTime() ? start + m_clip->maxDuration() : qMin(m_max, start + m_clip->maxDuration());
    if (start + duration > maxDuration) {
        m_view.clip_duration->setText(m_tc.getTimecode(maxDuration - start, m_fps));
    }
}

void ClipDurationDialog::slotCheckCrop() {
    int dur = m_tc.getFrameCount(m_view.clip_duration->text(), m_fps);
    int crop = m_tc.getFrameCount(m_view.crop_position->text(), m_fps);
    GenTime duration(dur, m_fps);
    GenTime cropStart(crop, m_fps);
    GenTime maxDuration = m_clip->maxDuration();
    if (cropStart + duration > maxDuration) {
        m_view.crop_position->setText(m_tc.getTimecode(maxDuration - duration, m_fps));
    }
}

void ClipDurationDialog::slotPosUp() {
    int position = m_tc.getFrameCount(m_view.clip_position->text(), m_fps);
    //if (duration >= m_clip->duration().frames(m_fps)) return;
    position ++;
    m_view.clip_position->setText(m_tc.getTimecode(GenTime(position, m_fps), m_fps));
}

void ClipDurationDialog::slotPosDown() {
    int position = m_tc.getFrameCount(m_view.clip_position->text(), m_fps);
    //if (duration >= m_clip->duration().frames(m_fps)) return;
    position --;
    m_view.clip_position->setText(m_tc.getTimecode(GenTime(position, m_fps), m_fps));
}

void ClipDurationDialog::slotDurUp() {
    int duration = m_tc.getFrameCount(m_view.clip_duration->text(), m_fps);
    int crop = m_tc.getFrameCount(m_view.crop_position->text(), m_fps);
    if (duration + crop > m_clip->maxDuration().frames(m_fps)) return;
    duration ++;
    m_view.clip_duration->setText(m_tc.getTimecode(GenTime(duration, m_fps), m_fps));
}

void ClipDurationDialog::slotDurDown() {
    int duration = m_tc.getFrameCount(m_view.clip_duration->text(), m_fps);
    if (duration <= 0) return;
    duration --;
    m_view.clip_duration->setText(m_tc.getTimecode(GenTime(duration, m_fps), m_fps));
}

void ClipDurationDialog::slotCropUp() {
    int crop = m_tc.getFrameCount(m_view.crop_position->text(), m_fps);
    int duration = m_tc.getFrameCount(m_view.clip_duration->text(), m_fps);
    if (duration + crop > m_clip->maxDuration().frames(m_fps)) return;
    crop ++;
    m_view.crop_position->setText(m_tc.getTimecode(GenTime(crop, m_fps), m_fps));
}

void ClipDurationDialog::slotCropDown() {
    int crop = m_tc.getFrameCount(m_view.crop_position->text(), m_fps);
    if (crop <= 0) return;
    crop --;
    m_view.crop_position->setText(m_tc.getTimecode(GenTime(crop, m_fps), m_fps));
}

GenTime ClipDurationDialog::startPos() const {
    int pos = m_tc.getFrameCount(m_view.clip_position->text(), m_fps);
    return GenTime(pos, m_fps);
}

GenTime ClipDurationDialog::cropStart() const {
    int pos = m_tc.getFrameCount(m_view.crop_position->text(), m_fps);
    return GenTime(pos, m_fps);
}

GenTime ClipDurationDialog::duration() const {
    int pos = m_tc.getFrameCount(m_view.clip_duration->text(), m_fps);
    return GenTime(pos, m_fps);
}

void ClipDurationDialog::wheelEvent(QWheelEvent * event) {
    if (m_view.clip_position->underMouse()) {
        if (event->delta() > 0)
            slotPosUp();
        else
            slotPosDown();
    } else if (m_view.clip_duration->underMouse()) {
        if (event->delta() > 0)
            slotDurUp();
        else
            slotDurDown();
    } else if (m_view.crop_position->underMouse()) {
        if (event->delta() > 0)
            slotCropUp();
        else
            slotCropDown();
    }
}

#include "clipdurationdialog.moc"


