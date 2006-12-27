/*
	Copyright (C) 2006 Nicolas BOTTI <rududu at laposte.net>
	This file is part of the Musepack Winamp plugin.

	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include <windows.h>

#include "in2.h"
#include "mpc_player.h"

namespace winamp_musepack {

System::Void mpc_info::btnCancel_Click(System::Object^  sender, System::EventArgs^  e)
{
	Close();
}

System::Void mpc_info::btnReload_Click(System::Object^  sender, System::EventArgs^  e)
{
	player->loadTags(this);
}

System::Void mpc_info::btnUpdate_Click(System::Object^  sender, System::EventArgs^  e)
{
	Hide();
	player->writeTags(this);
	Close();
}

}

