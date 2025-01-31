// ReSharper disable CppParameterMayBeConst
#pragma once


namespace ed
{
	template <class Elem_>
	class PrefixBufBasic
		: public std::basic_streambuf<Elem_>
	{
	public:
		PrefixBufBasic(std::basic_string<Elem_> prefix, std::basic_streambuf<Elem_>* sBuf, bool prefixOnFirstLine)
			: prefix_(std::move(prefix))
			, sBuf_(sBuf)
			, needPrefix_(prefixOnFirstLine)
		{
		}

	    using IntType = typename std::basic_streambuf<Elem_>::int_type;

	private:
		int sync() override
		{
			return sBuf_->pubsync();
		}

		IntType overflow(IntType chr) override
		{
			if (chr != std::char_traits<char>::eof())
			{
				if (needPrefix_
					&& !prefix_.empty()
					&& prefix_.size() != sBuf_->sputn(prefix_.data(), prefix_.size()))
				{
					return std::char_traits<char>::eof();
				}
				needPrefix_ = chr == '\n';
			}
			return sBuf_->sputc(chr); // NOLINT(bugprone-narrowing-conversions, cppcoreguidelines-narrowing-conversions, clang-diagnostic-implicit-int-conversion)
		}

		std::basic_string<Elem_> prefix_;
		std::basic_streambuf<Elem_>* sBuf_;
		bool needPrefix_;
	};

	template <class Elem_>
	class PrefixOStreamBasic final
		: virtual PrefixBufBasic<Elem_>
		, public virtual std::basic_ostream<Elem_>
	{
	public:
		// ReSharper disable once CppParameterMayBeConstPtrOrRef
		PrefixOStreamBasic(std::basic_string<Elem_> const& prefix, std::basic_ostream<Elem_>& out, bool prefixOnFirstLine = false)
			: PrefixBufBasic<Elem_>(prefix, out.rdbuf(), prefixOnFirstLine)
			, std::basic_ostream<Elem_>(static_cast<std::basic_streambuf<Elem_>*>(this))
		{
		}
	};

	typedef PrefixOStreamBasic<char> PrefixOStream;
	typedef PrefixOStreamBasic<wchar_t> PrefixOStreamW;
}
