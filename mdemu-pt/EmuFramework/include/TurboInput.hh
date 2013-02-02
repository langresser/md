/*  This file is part of Imagine.

	Imagine is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	Imagine is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with Imagine.  If not, see <http://www.gnu.org/licenses/> */

#pragma once

struct TurboInput
{
	struct Action
	{
		uint action;
		uchar player;

		bool operator ==(uint rhs) const
		{
			return action == rhs;
		}
	};

	Action activeAction[5];

	void init()
	{
		mem_zero(activeAction);
	}

	void addEvent(uchar player, uint action)
	{
		Action *slot = mem_findFirstZeroValue(activeAction);
		if(slot)
		{
			slot->action = action;
			slot->player = player;
			logMsg("added turbo event for player %d, action %d", player, action);
		}
	}

	void removeEvent(uchar player, uint action)
	{
		forEachInArray(activeAction, e)
		{
			if(e->action == action && e->player ==  player)
			{
				e->action = 0;
				logMsg("removed turbo event for player %d, action %d", player, action);
			}
		}
	}
};
