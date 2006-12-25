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

#pragma once

using namespace System;
using namespace System::ComponentModel;
using namespace System::Collections;
using namespace System::Windows::Forms;
using namespace System::Data;
using namespace System::Drawing;

class mpc_player;

namespace winamp_musepack {

	/// <summary>
	/// Summary for mpc_info
	///
	/// WARNING: If you change the name of this class, you will need to change the
	///          'Resource File Name' property for the managed resource compiler tool
	///          associated with all .resx files this class depends on.  Otherwise,
	///          the designers will not be able to interact properly with localized
	///          resources associated with this form.
	/// </summary>
	public ref class mpc_info : public System::Windows::Forms::Form
	{
	public:
		mpc_info(mpc_player * init_player)
		{
			InitializeComponent();
			player = init_player;
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~mpc_info()
		{
			if (components)
			{
				delete components;
			}
		}
	private:
	System::Windows::Forms::GroupBox^  groupBox1;
	System::Windows::Forms::Label^  label5;
	System::Windows::Forms::Label^  label4;
	System::Windows::Forms::Label^  label3;
	System::Windows::Forms::Label^  label2;
	System::Windows::Forms::Label^  label1;
	System::Windows::Forms::GroupBox^  groupBox2;
	System::Windows::Forms::Label^  label6;
	System::Windows::Forms::Label^  label7;




	mpc_player * player;
			 
	public:
	System::Windows::Forms::TextBox^  txtTrack;
	System::Windows::Forms::TextBox^  txtYear;
	System::Windows::Forms::TextBox^  txtAlbum;
	System::Windows::Forms::TextBox^  txtArtist;
	System::Windows::Forms::TextBox^  txtTitle;
	System::Windows::Forms::TextBox^  txtComment;
	System::Windows::Forms::ComboBox^  comboGenre;
	System::Windows::Forms::Label^  lblStreamInfo;
	private: System::Windows::Forms::Button^  btnUpdate;
	public: 
	private: System::Windows::Forms::Button^  btnCancel;
	private: System::Windows::Forms::Button^  btnReload;

	private:
		/// <summary>
		/// Required designer variable.
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		void InitializeComponent(void)
		{
			this->groupBox1 = (gcnew System::Windows::Forms::GroupBox());
			this->btnUpdate = (gcnew System::Windows::Forms::Button());
			this->btnCancel = (gcnew System::Windows::Forms::Button());
			this->btnReload = (gcnew System::Windows::Forms::Button());
			this->label7 = (gcnew System::Windows::Forms::Label());
			this->comboGenre = (gcnew System::Windows::Forms::ComboBox());
			this->txtComment = (gcnew System::Windows::Forms::TextBox());
			this->label6 = (gcnew System::Windows::Forms::Label());
			this->txtTrack = (gcnew System::Windows::Forms::TextBox());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->txtYear = (gcnew System::Windows::Forms::TextBox());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->txtAlbum = (gcnew System::Windows::Forms::TextBox());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->txtArtist = (gcnew System::Windows::Forms::TextBox());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->txtTitle = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->groupBox2 = (gcnew System::Windows::Forms::GroupBox());
			this->lblStreamInfo = (gcnew System::Windows::Forms::Label());
			this->groupBox1->SuspendLayout();
			this->groupBox2->SuspendLayout();
			this->SuspendLayout();
			// 
			// groupBox1
			// 
			this->groupBox1->Controls->Add(this->btnUpdate);
			this->groupBox1->Controls->Add(this->btnCancel);
			this->groupBox1->Controls->Add(this->btnReload);
			this->groupBox1->Controls->Add(this->label7);
			this->groupBox1->Controls->Add(this->comboGenre);
			this->groupBox1->Controls->Add(this->txtComment);
			this->groupBox1->Controls->Add(this->label6);
			this->groupBox1->Controls->Add(this->txtTrack);
			this->groupBox1->Controls->Add(this->label5);
			this->groupBox1->Controls->Add(this->txtYear);
			this->groupBox1->Controls->Add(this->label4);
			this->groupBox1->Controls->Add(this->txtAlbum);
			this->groupBox1->Controls->Add(this->label3);
			this->groupBox1->Controls->Add(this->txtArtist);
			this->groupBox1->Controls->Add(this->label2);
			this->groupBox1->Controls->Add(this->txtTitle);
			this->groupBox1->Controls->Add(this->label1);
			this->groupBox1->Location = System::Drawing::Point(12, 12);
			this->groupBox1->Name = L"groupBox1";
			this->groupBox1->Size = System::Drawing::Size(354, 212);
			this->groupBox1->TabIndex = 0;
			this->groupBox1->TabStop = false;
			this->groupBox1->Text = L"Tag";
			// 
			// btnUpdate
			// 
			this->btnUpdate->Location = System::Drawing::Point(6, 183);
			this->btnUpdate->Name = L"btnUpdate";
			this->btnUpdate->Size = System::Drawing::Size(84, 23);
			this->btnUpdate->TabIndex = 16;
			this->btnUpdate->Text = L"Update";
			this->btnUpdate->UseVisualStyleBackColor = true;
			this->btnUpdate->Click += gcnew System::EventHandler(this, &mpc_info::btnUpdate_Click);
			// 
			// btnCancel
			// 
			this->btnCancel->Location = System::Drawing::Point(135, 183);
			this->btnCancel->Name = L"btnCancel";
			this->btnCancel->Size = System::Drawing::Size(84, 23);
			this->btnCancel->TabIndex = 15;
			this->btnCancel->Text = L"Cancel";
			this->btnCancel->UseVisualStyleBackColor = true;
			this->btnCancel->Click += gcnew System::EventHandler(this, &mpc_info::btnCancel_Click);
			// 
			// btnReload
			// 
			this->btnReload->Location = System::Drawing::Point(264, 183);
			this->btnReload->Name = L"btnReload";
			this->btnReload->Size = System::Drawing::Size(84, 23);
			this->btnReload->TabIndex = 14;
			this->btnReload->Text = L"Reload";
			this->btnReload->UseVisualStyleBackColor = true;
			this->btnReload->Click += gcnew System::EventHandler(this, &mpc_info::btnReload_Click);
			// 
			// label7
			// 
			this->label7->AutoSize = true;
			this->label7->Location = System::Drawing::Point(186, 100);
			this->label7->Name = L"label7";
			this->label7->Size = System::Drawing::Size(36, 13);
			this->label7->TabIndex = 13;
			this->label7->Text = L"Genre";
			this->label7->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// comboGenre
			// 
			this->comboGenre->FormattingEnabled = true;
			this->comboGenre->Location = System::Drawing::Point(228, 96);
			this->comboGenre->Name = L"comboGenre";
			this->comboGenre->Size = System::Drawing::Size(120, 21);
			this->comboGenre->TabIndex = 12;
			// 
			// txtComment
			// 
			this->txtComment->Location = System::Drawing::Point(63, 123);
			this->txtComment->Multiline = true;
			this->txtComment->Name = L"txtComment";
			this->txtComment->Size = System::Drawing::Size(285, 54);
			this->txtComment->TabIndex = 11;
			// 
			// label6
			// 
			this->label6->AutoSize = true;
			this->label6->Location = System::Drawing::Point(6, 126);
			this->label6->Name = L"label6";
			this->label6->Size = System::Drawing::Size(51, 13);
			this->label6->TabIndex = 10;
			this->label6->Text = L"Comment";
			this->label6->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtTrack
			// 
			this->txtTrack->Location = System::Drawing::Point(160, 97);
			this->txtTrack->Name = L"txtTrack";
			this->txtTrack->Size = System::Drawing::Size(21, 20);
			this->txtTrack->TabIndex = 9;
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Location = System::Drawing::Point(109, 100);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(45, 13);
			this->label5->TabIndex = 8;
			this->label5->Text = L"Track #";
			this->label5->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtYear
			// 
			this->txtYear->Location = System::Drawing::Point(63, 97);
			this->txtYear->Name = L"txtYear";
			this->txtYear->Size = System::Drawing::Size(40, 20);
			this->txtYear->TabIndex = 7;
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Location = System::Drawing::Point(28, 100);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(29, 13);
			this->label4->TabIndex = 6;
			this->label4->Text = L"Year";
			this->label4->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtAlbum
			// 
			this->txtAlbum->Location = System::Drawing::Point(63, 71);
			this->txtAlbum->Name = L"txtAlbum";
			this->txtAlbum->Size = System::Drawing::Size(285, 20);
			this->txtAlbum->TabIndex = 5;
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Location = System::Drawing::Point(21, 74);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(36, 13);
			this->label3->TabIndex = 4;
			this->label3->Text = L"Album";
			this->label3->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtArtist
			// 
			this->txtArtist->Location = System::Drawing::Point(63, 45);
			this->txtArtist->Name = L"txtArtist";
			this->txtArtist->Size = System::Drawing::Size(285, 20);
			this->txtArtist->TabIndex = 3;
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Location = System::Drawing::Point(27, 48);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(30, 13);
			this->label2->TabIndex = 2;
			this->label2->Text = L"Artist";
			this->label2->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// txtTitle
			// 
			this->txtTitle->Location = System::Drawing::Point(63, 19);
			this->txtTitle->Name = L"txtTitle";
			this->txtTitle->Size = System::Drawing::Size(285, 20);
			this->txtTitle->TabIndex = 1;
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Location = System::Drawing::Point(30, 22);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(27, 13);
			this->label1->TabIndex = 0;
			this->label1->Text = L"Title";
			this->label1->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
			// 
			// groupBox2
			// 
			this->groupBox2->Controls->Add(this->lblStreamInfo);
			this->groupBox2->Location = System::Drawing::Point(372, 12);
			this->groupBox2->Name = L"groupBox2";
			this->groupBox2->Size = System::Drawing::Size(149, 212);
			this->groupBox2->TabIndex = 1;
			this->groupBox2->TabStop = false;
			this->groupBox2->Text = L"Stream";
			// 
			// lblStreamInfo
			// 
			this->lblStreamInfo->AutoEllipsis = true;
			this->lblStreamInfo->Location = System::Drawing::Point(6, 16);
			this->lblStreamInfo->Name = L"lblStreamInfo";
			this->lblStreamInfo->Size = System::Drawing::Size(137, 190);
			this->lblStreamInfo->TabIndex = 0;
			this->lblStreamInfo->Text = L"lblStreamInfo";
			// 
			// mpc_info
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(534, 237);
			this->Controls->Add(this->groupBox2);
			this->Controls->Add(this->groupBox1);
			this->Name = L"mpc_info";
			this->Text = L"Musepack file information";
			this->groupBox1->ResumeLayout(false);
			this->groupBox1->PerformLayout();
			this->groupBox2->ResumeLayout(false);
			this->ResumeLayout(false);

		}
#pragma endregion

	private :
	System::Void btnCancel_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void btnReload_Click(System::Object^  sender, System::EventArgs^  e);
	System::Void btnUpdate_Click(System::Object^  sender, System::EventArgs^  e);
};
}
