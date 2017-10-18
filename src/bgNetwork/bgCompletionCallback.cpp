#include "stdafx.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "base/compiler_specific.h"
#include "base/message_loop.h"
#include "net/base/io_buffer.h"

#include "bgCompletionCallback.h"

namespace internal {

	void bgCompletionCallbackBaseInternal::DidSetResult() {
		have_result_ = true;
		if (waiting_for_result_)
			MessageLoop::current()->Quit();
	}

	void bgCompletionCallbackBaseInternal::WaitForResult() {
		DCHECK(!waiting_for_result_);
		while (!have_result_) {
			waiting_for_result_ = true;
			MessageLoop::current()->Run();
			waiting_for_result_ = false;
		}
		have_result_ = false;  // Auto-reset for next callback.
	}

	bgCompletionCallbackBaseInternal::bgCompletionCallbackBaseInternal()
		: have_result_(false),
		waiting_for_result_(false) {
	}

}  // namespace internal


bgCompletionCallback::bgCompletionCallback()
	: callback_(base::Bind(&bgCompletionCallback::SetResult,
	base::Unretained(this))) {
}

bgCompletionCallback::~bgCompletionCallback() {
}

bgInt64CompletionCallback::bgInt64CompletionCallback()
	: callback_(base::Bind(&bgInt64CompletionCallback::SetResult,
	base::Unretained(this))) {
}

bgInt64CompletionCallback::~bgInt64CompletionCallback() {
}

ReleaseBufferCompletionCallback::ReleaseBufferCompletionCallback(
	net::IOBuffer* buffer) : buffer_(buffer) {
}

ReleaseBufferCompletionCallback::~ReleaseBufferCompletionCallback() {
}

void ReleaseBufferCompletionCallback::SetResult(int result) {
	if (!buffer_->HasOneRef())
		result = net::ERR_FAILED;
	bgCompletionCallback::SetResult(result);
}