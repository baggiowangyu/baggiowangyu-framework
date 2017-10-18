#ifndef _BG_COMPLETION_CALLBACK_H_
#define _BG_COMPLETION_CALLBACK_H_

#include "base/compiler_specific.h"
#include "base/tuple.h"
#include "net/base/completion_callback.h"
#include "net/base/net_errors.h"

//-----------------------------------------------------------------------------
// completion callback helper

// A helper class for completion callbacks, designed to make it easy to run
// tests involving asynchronous operations.  Just call WaitForResult to wait
// for the asynchronous operation to complete.
//
// NOTE: Since this runs a message loop to wait for the completion callback,
// there could be other side-effects resulting from WaitForResult.  For this
// reason, this class is probably not ideal for a general application.
//

namespace internal {

	class bgCompletionCallbackBaseInternal {
	public:
		bool have_result() const { return have_result_; }

	protected:
		bgCompletionCallbackBaseInternal();
		void DidSetResult();
		void WaitForResult();

		bool have_result_;
		bool waiting_for_result_;

	private:
		DISALLOW_COPY_AND_ASSIGN(bgCompletionCallbackBaseInternal);
	};

	template <typename R>
	class bgCompletionCallbackTemplate
		: public bgCompletionCallbackBaseInternal {
	public:
		virtual ~bgCompletionCallbackTemplate() {}

		R WaitForResult() {
			bgCompletionCallbackBaseInternal::WaitForResult();
			return result_;
		}

		R GetResult(R result) {
			if (net::ERR_IO_PENDING != result)
				return result;
			return WaitForResult();
		}

	protected:
		// Override this method to gain control as the callback is running.
		virtual void SetResult(R result) {
			result_ = result;
			DidSetResult();
		}

		bgCompletionCallbackTemplate() : result_(R()) {}
		R result_;

	private:
		DISALLOW_COPY_AND_ASSIGN(bgCompletionCallbackTemplate);
	};

};

// Base class overridden by custom implementations of TestCompletionCallback.
typedef internal::bgCompletionCallbackTemplate<int>
	bgCompletionCallbackBase;

typedef internal::bgCompletionCallbackTemplate<int64>
	bgInt64CompletionCallbackBase;

class bgCompletionCallback : public bgCompletionCallbackBase {
public:
	bgCompletionCallback();
	virtual ~bgCompletionCallback();

	const net::CompletionCallback& callback() const { return callback_; }

private:
	const net::CompletionCallback callback_;

	DISALLOW_COPY_AND_ASSIGN(bgCompletionCallback);
};

class bgInt64CompletionCallback : public bgInt64CompletionCallbackBase {
public:
	bgInt64CompletionCallback();
	virtual ~bgInt64CompletionCallback();

	const net::Int64CompletionCallback& callback() const { return callback_; }

private:
	const net::Int64CompletionCallback callback_;

	DISALLOW_COPY_AND_ASSIGN(bgInt64CompletionCallback);
};

// Makes sure that the buffer is not referenced when the callback runs.
class ReleaseBufferCompletionCallback: public bgCompletionCallback {
public:
	explicit ReleaseBufferCompletionCallback(net::IOBuffer* buffer);
	virtual ~ReleaseBufferCompletionCallback();

private:
	virtual void SetResult(int result) OVERRIDE;

	net::IOBuffer* buffer_;
	DISALLOW_COPY_AND_ASSIGN(ReleaseBufferCompletionCallback);
};

#endif
