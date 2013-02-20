/***************************************************************************
 *   Copyright (C) 2009-2010 Virginia Tech Real-Time Systems Lab
 *   Written by Matthew Dellinger
 *   mdelling@vt.edu
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
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef RT_SCHED_LIST_H
#define RT_SCHED_LIST_H

#include <QString>
#include <QDebug>

#include <string>
#include <unistd.h>

#include <chronos/chronos.h>

using namespace std;

struct SchedMap {
	int mask;
	QString name;
};

class SchedSelector {

	public:
	SchedSelector() {
		initializeSchedSelector(0);
	}

	SchedSelector(int prio) {
		initializeSchedSelector(prio);
	}

	void initializeSchedSelector(int prio) {
		sched_base_ = 0;
		sched_options_ = 0;
		global_prio_ = prio;
		initializeSchedList();
	}

	int count() {
		return sched_list.count();
	}

	QString AlgoName(int i) {
		return sched_list[i].name;
	}

	QString CurrentAlgoName() {
		for(int i = 0; i < sched_list.count(); i++) {
			if (sched_base_ == sched_list[i].mask)
				return sched_list[i].name;
		}

		return QString("Unknown");
	}

	void setBaseSched(QString scheduler) {
		QString base = scheduler.toUpper();

		for(int i = 0; i < sched_list.count(); i++) {
			if (base == sched_list[i].name)
				sched_base_ = sched_list[i].mask;
		}
	}

	void setSchedOption(int option, bool value) {
		if(value)
			sched_options_ |= option;
		else
			sched_options_ &= ~option;	
	}

	void setGlobalPrio(int prio) {
		global_prio_ = prio;
	}

	int Sched() {
		return (sched_base_ | sched_options_);
	}

	int SchedPriority() {
		if(sched_base_ & SCHED_GLOBAL_MASK)
			return global_prio_;
		else
			return -1;
	}

	private:
	//Private functions
	void initializeSchedList() {
		SchedMap sched;

		//NONE
		sched.mask = -1;
		sched.name = "NONE";
		sched_list.append(sched);

		//FIFO
		sched.mask = SCHED_RT_FIFO;
		sched.name = "FIFO";
		sched_list.append(sched);

		//RMA
		sched.mask = SCHED_RT_RMA;
		sched.name = "RMA";
		sched_list.append(sched);

		//EDF
		sched.mask = SCHED_RT_EDF;
		sched.name = "EDF";
		sched_list.append(sched);

		//HVDF
		sched.mask = SCHED_RT_HVDF;
		sched.name = "HVDF";
		sched_list.append(sched);

		//LBESA
		sched.mask = SCHED_RT_LBESA;
		sched.name = "LBESA";
		sched_list.append(sched);

		//DASA_ND
		sched.mask = SCHED_RT_DASA_ND;
		sched.name = "DASA_ND";
		sched_list.append(sched);

		//DASA
		sched.mask = SCHED_RT_DASA;
		sched.name = "DASA";
		sched_list.append(sched);

		//G_FIFO
		sched.mask = SCHED_RT_GFIFO;
		sched.name = "G_FIFO";
		sched_list.append(sched);

		//G_RMA
		sched.mask = SCHED_RT_GRMA;
		sched.name = "G_RMA";
		sched_list.append(sched);

		//G_EDF
		sched.mask = SCHED_RT_GEDF;
		sched.name = "G_EDF";
		sched_list.append(sched);

		//G_NP_EDF
/*
		sched.mask = SCHED_RT_GNPEDF;
		sched.name = "G_NP_EDF";
		sched_list.append(sched);
*/
		//G_HVDF
		sched.mask = SCHED_RT_GHVDF;
		sched.name = "G_HVDF";
		sched_list.append(sched);

		//G_MUA
		sched.mask = SCHED_RT_GMUA;
		sched.name = "G_MUA";
		sched_list.append(sched);

		//NG_GUA
		sched.mask = SCHED_RT_GGUA;
		sched.name = "G_GUA";
		sched_list.append(sched);

		//G_GUA
		sched.mask = SCHED_RT_NGGUA;
		sched.name = "NG_GUA";
		sched_list.append(sched);
	}

	//Private members
	int sched_base_;
	int sched_options_;
	int global_prio_;

	QList<SchedMap> sched_list;
};

#endif
