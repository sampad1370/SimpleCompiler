#include "ErrorManager.h"
using namespace std;

namespace CompilerPackage
{
	namespace Error
	{
		//ErrorManager g_ErrorManager;
		ErrorManager::ErrorManager()
		{
		}


		ErrorManager::~ErrorManager()
		{
		}

		void ErrorManager::Add(BaseError* error)
		{
			m_Queue.push_back(error);
		}

		void ErrorManager::ReportToFile(string filename)
		{
			fstream fs;
			String EndMessage = "Error Occurred!";
			fs.open(filename + ".lerr", ios::out);
			for (auto err : m_Queue)
			{
				switch (err->ErrType)
				{
					case ErrorType::Internal:
						fs << "Internal Error Occurred!" << ":\n" << err->Message << endl;
					break;
					case ErrorType::Compiler:
					break;
					case ErrorType::LexicalAnalyser:
					{
													   LexicalAnalyserError* error = (LexicalAnalyserError*) err;
													   fs << EndMessage << ":\nRow: " << error->Row << ",Column: " << error->Col << ", " << error->Message << endl;
					}
					break;
				}
			}
		}

		bool ErrorManager::HasError(){ return m_Queue.size() != 0; }

		BaseError* ErrorManager::GetFirstError()
		{
			if (m_Queue.size() > 0)
				return m_Queue[0];
			else
				return nullptr;
		}
	}
}