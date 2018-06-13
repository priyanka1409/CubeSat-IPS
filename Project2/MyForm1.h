#pragma once
#include "ImgMatcher.h"
#include "string.h"
#include <msclr/marshal_cppstd.h>

namespace Project2 {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	ImgMatcher matcher;

	/// <summary>
	/// Summary for MyForm
	/// </summary>
	public ref class MyForm1 : public System::Windows::Forms::Form
	{
	public:
		MyForm1(void)
		{
			InitializeComponent();
			//
			//TODO: Add the constructor code here
			//
		}

	protected:
		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		~MyForm1()
		{
			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::Button^  button1;
	private: System::Windows::Forms::TextBox^  textBox1;
	private: System::Windows::Forms::Label^  label1;
	private: System::Windows::Forms::Label^  label2;
	private: System::Windows::Forms::Label^  label3;
	private: System::Windows::Forms::TextBox^  textBox2;
	private: System::Windows::Forms::Button^  button2;
	private: System::Windows::Forms::Button^  button3;
	private: System::Windows::Forms::Label^  label4;
	private: System::Windows::Forms::TextBox^  textBox3;
	private: System::Windows::Forms::Button^  button4;
	private: System::Windows::Forms::Label^  label5;
	private: System::Windows::Forms::TextBox^  textBox4;

			 //private: ImgMatcher matcher;

	protected:

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
			this->button1 = (gcnew System::Windows::Forms::Button());
			this->textBox1 = (gcnew System::Windows::Forms::TextBox());
			this->label1 = (gcnew System::Windows::Forms::Label());
			this->label2 = (gcnew System::Windows::Forms::Label());
			this->label3 = (gcnew System::Windows::Forms::Label());
			this->textBox2 = (gcnew System::Windows::Forms::TextBox());
			this->button2 = (gcnew System::Windows::Forms::Button());
			this->button3 = (gcnew System::Windows::Forms::Button());
			this->label4 = (gcnew System::Windows::Forms::Label());
			this->textBox3 = (gcnew System::Windows::Forms::TextBox());
			this->button4 = (gcnew System::Windows::Forms::Button());
			this->label5 = (gcnew System::Windows::Forms::Label());
			this->textBox4 = (gcnew System::Windows::Forms::TextBox());
			this->SuspendLayout();
			// 
			// button1
			// 
			this->button1->Location = System::Drawing::Point(466, 64);
			this->button1->Name = L"button1";
			this->button1->Size = System::Drawing::Size(26, 20);
			this->button1->TabIndex = 0;
			this->button1->Text = L"...";
			this->button1->UseVisualStyleBackColor = true;
			this->button1->Click += gcnew System::EventHandler(this, &MyForm1::button1_Click);
			// 
			// textBox1
			// 
			this->textBox1->Location = System::Drawing::Point(238, 64);
			this->textBox1->Name = L"textBox1";
			this->textBox1->Size = System::Drawing::Size(222, 20);
			this->textBox1->TabIndex = 1;
			this->textBox1->TextChanged += gcnew System::EventHandler(this, &MyForm1::textBox1_TextChanged);
			// 
			// label1
			// 
			this->label1->AutoSize = true;
			this->label1->Font = (gcnew System::Drawing::Font(L"Calibri", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label1->Location = System::Drawing::Point(86, 67);
			this->label1->Name = L"label1";
			this->label1->Size = System::Drawing::Size(146, 15);
			this->label1->TabIndex = 2;
			this->label1->Text = L"Source Image (Raw) Path:";
			this->label1->Click += gcnew System::EventHandler(this, &MyForm1::label1_Click);
			// 
			// label2
			// 
			this->label2->AutoSize = true;
			this->label2->Font = (gcnew System::Drawing::Font(L"Candara", 15.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label2->Location = System::Drawing::Point(277, 19);
			this->label2->Name = L"label2";
			this->label2->Size = System::Drawing::Size(121, 26);
			this->label2->TabIndex = 3;
			this->label2->Text = L"CubeSat IPS";
			this->label2->Click += gcnew System::EventHandler(this, &MyForm1::label2_Click);
			// 
			// label3
			// 
			this->label3->AutoSize = true;
			this->label3->Font = (gcnew System::Drawing::Font(L"Calibri", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label3->Location = System::Drawing::Point(51, 103);
			this->label3->Name = L"label3";
			this->label3->Size = System::Drawing::Size(181, 15);
			this->label3->TabIndex = 6;
			this->label3->Text = L"GeoTiff Image (Reference) Path:";
			// 
			// textBox2
			// 
			this->textBox2->Location = System::Drawing::Point(238, 101);
			this->textBox2->Name = L"textBox2";
			this->textBox2->Size = System::Drawing::Size(222, 20);
			this->textBox2->TabIndex = 5;
			this->textBox2->TextChanged += gcnew System::EventHandler(this, &MyForm1::textBox2_TextChanged);
			// 
			// button2
			// 
			this->button2->Location = System::Drawing::Point(466, 100);
			this->button2->Name = L"button2";
			this->button2->Size = System::Drawing::Size(26, 20);
			this->button2->TabIndex = 4;
			this->button2->Text = L"...";
			this->button2->UseVisualStyleBackColor = true;
			this->button2->Click += gcnew System::EventHandler(this, &MyForm1::button2_Click);
			// 
			// button3
			// 
			this->button3->Font = (gcnew System::Drawing::Font(L"Microsoft Sans Serif", 9.75F, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->button3->Location = System::Drawing::Point(272, 264);
			this->button3->Name = L"button3";
			this->button3->Size = System::Drawing::Size(147, 33);
			this->button3->TabIndex = 7;
			this->button3->Text = L"Create a GeoTiff!";
			this->button3->UseVisualStyleBackColor = true;
			this->button3->Click += gcnew System::EventHandler(this, &MyForm1::button3_Click);
			// 
			// label4
			// 
			this->label4->AutoSize = true;
			this->label4->Font = (gcnew System::Drawing::Font(L"Calibri", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label4->Location = System::Drawing::Point(107, 140);
			this->label4->Name = L"label4";
			this->label4->Size = System::Drawing::Size(125, 15);
			this->label4->TabIndex = 10;
			this->label4->Text = L"Output Image Folder:";
			this->label4->Click += gcnew System::EventHandler(this, &MyForm1::label4_Click);
			// 
			// textBox3
			// 
			this->textBox3->Location = System::Drawing::Point(238, 138);
			this->textBox3->Name = L"textBox3";
			this->textBox3->Size = System::Drawing::Size(222, 20);
			this->textBox3->TabIndex = 9;
			this->textBox3->TextChanged += gcnew System::EventHandler(this, &MyForm1::textBox3_TextChanged);
			// 
			// button4
			// 
			this->button4->Location = System::Drawing::Point(466, 137);
			this->button4->Name = L"button4";
			this->button4->Size = System::Drawing::Size(26, 20);
			this->button4->TabIndex = 8;
			this->button4->Text = L"...";
			this->button4->UseVisualStyleBackColor = true;
			this->button4->Click += gcnew System::EventHandler(this, &MyForm1::button4_Click);
			// 
			// label5
			// 
			this->label5->AutoSize = true;
			this->label5->Font = (gcnew System::Drawing::Font(L"Calibri", 9.75F, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point,
				static_cast<System::Byte>(0)));
			this->label5->Location = System::Drawing::Point(123, 176);
			this->label5->Name = L"label5";
			this->label5->Size = System::Drawing::Size(109, 15);
			this->label5->TabIndex = 12;
			this->label5->Text = L"Output File Name:";
			// 
			// textBox4
			// 
			this->textBox4->Location = System::Drawing::Point(238, 174);
			this->textBox4->Name = L"textBox4";
			this->textBox4->Size = System::Drawing::Size(222, 20);
			this->textBox4->TabIndex = 11;
			this->textBox4->TextChanged += gcnew System::EventHandler(this, &MyForm1::textBox4_TextChanged);
			// 
			// MyForm1
			// 
			this->AutoScaleDimensions = System::Drawing::SizeF(6, 13);
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->ClientSize = System::Drawing::Size(680, 327);
			this->Controls->Add(this->label5);
			this->Controls->Add(this->textBox4);
			this->Controls->Add(this->label4);
			this->Controls->Add(this->textBox3);
			this->Controls->Add(this->button4);
			this->Controls->Add(this->button3);
			this->Controls->Add(this->label3);
			this->Controls->Add(this->textBox2);
			this->Controls->Add(this->button2);
			this->Controls->Add(this->label2);
			this->Controls->Add(this->label1);
			this->Controls->Add(this->textBox1);
			this->Controls->Add(this->button1);
			this->Name = L"MyForm1";
			this->Text = L"Creating IPS Output";
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion
	private: System::Void button1_Click(System::Object^  sender, System::EventArgs^  e) {

		OpenFileDialog openFileDialog;
		openFileDialog.Filter = "Tiff Files|*.tif";

		if (openFileDialog.ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			textBox1->Text = openFileDialog.FileName;
		}

	}
	private: System::Void label1_Click(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void label2_Click(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void button2_Click(System::Object^  sender, System::EventArgs^  e) {

		OpenFileDialog openFileDialog;

		openFileDialog.Filter = "Tiff Files|*.tif";

		if (openFileDialog.ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			textBox2->Text = openFileDialog.FileName;
		}
	}
	private: System::Void button3_Click(System::Object^  sender, System::EventArgs^  e) {
		std::cout << std::endl << "Creating your GeoTiff..." << std::endl << std::endl;
		matcher.matchImgs();
	}
	private: System::Void label4_Click(System::Object^  sender, System::EventArgs^  e) {
	}
	private: System::Void textBox3_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		std::string outPath = msclr::interop::marshal_as< std::string >(textBox3->Text);
		matcher.setOutPath(outPath);
	}
	private: System::Void button4_Click(System::Object^  sender, System::EventArgs^  e) {
		FolderBrowserDialog folderBrowserDialog;

		if (folderBrowserDialog.ShowDialog() == System::Windows::Forms::DialogResult::OK) {
			textBox3->Text = folderBrowserDialog.SelectedPath;
		}
	}
	private: System::Void textBox4_TextChanged(System::Object^  sender, System::EventArgs^  e) {
		String^ fName = textBox4->Text;
		std::string fileName = msclr::interop::marshal_as< std::string >(fName);
		matcher.setFileName(fileName);
	}
private: System::Void textBox1_TextChanged(System::Object^  sender, System::EventArgs^  e) {
	std::string objArg = msclr::interop::marshal_as< std::string >(textBox1->Text);
	matcher.setObjMat(objArg);
}
private: System::Void textBox2_TextChanged(System::Object^  sender, System::EventArgs^  e) {
	std::string sceneArg = msclr::interop::marshal_as< std::string >(textBox2->Text);
	matcher.setScMat(sceneArg);
	//matcher.setOutPath(sceneArg);
}
};
}
