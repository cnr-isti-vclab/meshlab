#pragma once

namespace SyntopiaCore {
	namespace Exceptions {	

		/// A base exception class.
		///
		/// When using Exceptions:
		///  (1) Throw temporaries   (throw Exception("Error occoured");)
		///  (2) Catch by reference  ( try {} catch (Exception& e) {} )
		///
		/// (Perhaps this ought to inherit from std::exception?)
		class Exception {

		public:
			/// Constructor. 
			Exception(QString message) : message(message) {};

			/// Returns the error message.
			QString getMessage() const { return message; }

		private:
			QString message;

		};

	}
}

