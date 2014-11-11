/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2014  David Edmundson <david@davidedmundson.co.uk>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "LogindWatcher.h"
#include "DaemonApp.h"
#include "SeatManager.h"

#include <QDebug>

LogindWatcher::LogindWatcher(QObject* parent):
    QObject(parent)
{
	registerTypes();

    PendingManager* pm = Manager::manager();
    connect(pm, &PendingInterfaceInternal::finished, this, [=]() {
        m_manager = pm->interface();
        managerLoaded();
    });
}

void LogindWatcher::managerLoaded()
{
    PendingSeats* ps = m_manager->listSeats();
    connect(ps, &PendingInterfaceInternal::finished, this, [=](){
        foreach(const SeatPtr &seat, ps->interfaces()) {
            newSeat(seat);
        }
    });
    connect(m_manager.data(), &Manager::seatAdded, this, &LogindWatcher::newSeat);
}

void LogindWatcher::newSeat(const SeatPtr& seat)
{
qDebug() << "new seat";
    //keep a reference to all seats so we keep watching for changes
    m_seats.insert(seat->id(), seat);
    if (seat->canGraphical()) {
        canGraphicalAdded(seat->id());
    }
    connect(seat.data(), &OrgFreedesktopLogin1SeatInterface::canGraphicalChanged, this, [=]() {
        if (seat->canGraphical()) {
            canGraphicalAdded(seat->id());
        } else {
            canGraphicalRemoved(seat->id());
        }
    });
}

void LogindWatcher::canGraphicalAdded(const QString &seat)
{
	qDebug() << "New graphical on " << seat;
    SDDM::DaemonApp::instance()->seatManager()->createSeat(seat);
}

void LogindWatcher::canGraphicalRemoved(const QString &seat)
{
    SDDM::DaemonApp::instance()->seatManager()->removeSeat(seat);
}
