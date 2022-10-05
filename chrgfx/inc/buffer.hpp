/**
 * @file buffer.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @brief Data buffer wrapper with templated iterators
 * @copyright ©2020 Motoi Productions / Released under MIT License
 *
 * Updates:
 * 20220410 Initial
 * 20220420 Added templated size() method
 * 20220921 Added vector constructor; changed pointer constructor to copy;
 * consolidated similar constructors; corrected bug with append; a number of
 * changes based on clang-tidy suggestions
 */
#ifndef __MOTOI__BUFFER_HPP
#define __MOTOI__BUFFER_HPP

#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <vector>

template <typename DataT = char>
class buffer
{
public:
	buffer() = delete;
	buffer & operator= (buffer const &) = delete;
	buffer & operator= (buffer &&) = delete;

	/**
	 * @brief Create an empty buffer with optional initial value
	 * @param size Number of DataT elements
	 */
	explicit buffer (size_t const size, char const initial = 0) :
			m_size {size},
			m_datasize {sizeof (DataT) * size},
			m_buffer {(DataT *) malloc (sizeof (DataT) * size)}
	{
		if (this->m_buffer == nullptr)
			throw std::runtime_error ("Failed to allocate buffer space");

		std::memset (m_buffer, initial, m_size);

#ifdef DEBUG
		std::cerr << __func__ << ": Created empty buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
							<< ", datasize " << this->m_datasize << std::endl;
#endif
	}

	/**
	 * @brief Copy from pointer constructor
	 * @param data Pointer to data
	 * @param size Size of existing data
	 */
	buffer (DataT * data, size_t const size) :
			m_size {size},
			m_datasize {sizeof (DataT) * size},
			m_buffer {(DataT *) malloc (sizeof (DataT) * size)}
	{
		if (this->m_buffer == nullptr)
			throw std::runtime_error ("Failed to allocate buffer space");

		std::memcpy (this->m_buffer, data, m_datasize);
#ifdef DEBUG
		std::cerr << __func__ << ": Copying existing buffer from " << std::showbase << std::hex << (std::size_t) data
							<< " to " << (std::size_t) this->m_buffer << ", datasize " << this->m_datasize << std::endl;
#endif
	}

	/**
	 * @brief Copy constructor
	 */
	buffer (buffer const & other) :
			buffer (other.m_buffer, other.m_size)
	{
	}

	/**
	 * @brief Move constructor
	 */
	buffer (buffer && other) noexcept :
			m_size {other.m_size},
			m_datasize {other.m_datasize},
			m_buffer {std::move (other.m_buffer)}
	{
#ifdef DEBUG
		std::cerr << __func__ << ": Moving existing buffer at " << std::showbase << std::hex << (std::size_t) other.m_buffer
							<< " to " << (std::size_t) this->m_buffer << ", datasize " << this->m_datasize << std::endl;
#endif
	};

	/**
	 * @brief Input stream constructor
	 * @param data Input stream
	 * @param block_size Stream read buffer (default 128kKiB)
	 */
	explicit buffer (std::istream & data, size_t const block_size = DEFAULT_BLOCK_SZ)
	{
#ifdef DEBUG
		std::cerr << __func__ << ": Reading data from input stream in " << std::showbase << std::hex
							<< (std::size_t) block_size << "byte blocks" << std::endl;
#endif
		stream_in (data, block_size);
	}

	/**
	 * @brief Vector constructor
	 * @param vector Vector of data
	 */
	buffer (std::vector<DataT> & vec) :
			buffer (vec.data(), vec.size())
	{
	}

	~buffer()
	{
		if (this->m_buffer != nullptr)
		{
#ifdef DEBUG
			std::cerr << __func__ << ": Freeing buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
								<< ", datasize " << this->m_datasize << std::endl;
#endif
			free (this->m_buffer);
		}
	}

	/**
	 * @return size_t size of the buffer *in bytes*
	 */
	[[nodiscard]] size_t datasize() const
	{
		return this->m_datasize;
	}

	/**
	 * @return size_t size of the buffer in type DataT
	 */
	[[nodiscard]] size_t size() const
	{
		return this->m_size;
	}

	template <typename SizeT = DataT>
	[[nodiscard]] size_t size() const
	{
		return this->m_datasize / sizeof (SizeT);
	}

	/**
	 * @return DataT const* pointer to buffer in memory
	 */
	[[nodiscard]] DataT const * data() const
	{
		return this->m_buffer;
	}

	/**
	 * @return DataT const* pointer to buffer in memory
	 */
	[[nodiscard]] DataT * data()
	{
		return this->m_buffer;
	}

	size_t resize (size_t const new_size)
	{
		if (new_size == 0)
			throw std::invalid_argument ("Cannot resize buffer to 0");

		if (new_size == this->m_size)
			return this->m_size;

		if (new_size > this->m_size)
			return append (new_size);

#ifdef DEBUG
		size_t prev_datasize = this->m_datasize;
#endif

		size_t new_size_bytes = new_size * sizeof (DataT);
		this->m_buffer = (DataT *) realloc (this->m_buffer, new_size_bytes);
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		this->m_datasize = new_size_bytes;
		this->m_size = new_size;

#ifdef DEBUG
		std::cerr << __func__ << ": Shrunk buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
							<< ", prev datasize " << prev_datasize << " new datasize " << this->m_datasize << std::endl;
#endif

		return this->m_size;
	}

	size_t append (size_t const size, char const initial = 0)
	{
		if (size == 0)
			return this->m_size;

		size_t additional_bytes = size * sizeof (DataT);
		this->m_buffer = (DataT *) realloc (this->m_buffer, (this->m_datasize + additional_bytes));
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		std::memset (this->m_buffer + this->m_datasize, additional_bytes, initial);

		this->m_datasize += additional_bytes;
		this->m_size += size;
#ifdef DEBUG
		std::cerr << __func__ << ": Grew buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
							<< ", new datasize " << this->m_datasize << std::endl;
#endif

		return this->m_size;
	}

	size_t append (buffer const & other)
	{
		if (other.size() == 0)
			return this->m_size;

#ifdef DEBUG
		std::cerr << __func__ << ": Appending data from other buffer at " << std::showbase << std::hex
							<< (std::size_t) other.m_buffer << ", datasize " << other.m_datasize << std::endl;
#endif

		this->m_buffer = (DataT *) realloc (this->m_buffer, this->m_datasize + other.m_datasize);
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		std::memcpy ((char *) (this->m_buffer) + this->m_datasize, (char *) other.m_buffer, other.m_datasize);

		this->m_datasize += other.m_datasize;
		this->m_size += other.m_size;

		return this->m_size;
	}

	size_t append (DataT * const other, size_t size)
	{
		if (size == 0)
			return this->m_size;

#ifdef DEBUG
		std::cerr << __func__ << ": Appending data via pointer to " << std::showbase << std::hex << (std::size_t) other
							<< ", datasize " << size << std::endl;
#endif

		this->m_buffer = (DataT *) realloc (this->m_buffer, this->m_datasize + size);
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		std::memcpy ((char *) (this->m_buffer) + this->m_datasize, (char *) other, size);

		this->m_datasize += size;
		this->m_size += size / sizeof (DataT);

		return this->m_size;
	}

	friend std::ostream & operator<< (std::ostream & out, const buffer & buffer)
	{
		std::copy (buffer.begin(), buffer.end(), std::ostream_iterator<DataT> (out));
		return out;
	}

	friend std::istream & operator>> (std::istream & in, buffer & buffer)
	{
		buffer->stream_in (in, DEFAULT_BLOCK_SZ);
		return in;
	}

	DataT & operator[] (size_t const index)
	{
		if (index > this->m_size)
			throw std::out_of_range ("Specified subscript out of range");

		return this->m_buffer[index];
	}

	DataT const & operator[] (size_t const index) const
	{
		if (index > this->m_size)
			throw std::out_of_range ("Specified subscript out of range");

		return this->m_buffer[index];
	}

	template <typename IterT = DataT>
	struct iterator
	{
	public:
		using self_type = iterator;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = IterT;
		using difference_type = std::ptrdiff_t;
		using pointer = IterT *;
		using reference = IterT &;

		explicit iterator (pointer ptr) :
				ptr (ptr)
		{
		}

		iterator() = default;

		~iterator() = default;

		iterator (iterator const & other) = default;

		iterator & operator= (self_type const & other) = default;

		iterator & operator= (IterT const & other)
		{
			ptr = &other;
		};

		self_type & operator++()
		{
			++ptr;
			return *this;
		}

		self_type operator++ (int)
		{
			self_type tmp = *this;
			++*this;
			return tmp;
		}

		bool operator== (self_type const & other) const
		{
			return ptr == other.ptre;
		}

		bool operator!= (self_type const & other) const
		{
			return ptr != other.ptr;
		}

		reference operator*() const
		{
			if (ptr == nullptr)
				throw std::bad_function_call();
			return *(ptr);
		}

		pointer operator->() const
		{
			if (ptr == nullptr)
				throw std::bad_function_call();
			return *(ptr);
		}

		self_type & operator--()
		{
			--ptr;
			return *this;
		}

		self_type operator-- (int)
		{
			self_type tmp = *this;
			--*this;
			return tmp;
		}

		self_type operator+ (difference_type offset) const
		{
			self_type tmp = *this;
			return tmp += offset;
		}

		self_type operator- (difference_type offset) const
		{
			self_type tmp = *this;
			return tmp -= offset;
		}

		difference_type operator- (self_type const & other) const
		{
			return (ptr - other.ptr);
		}

		bool operator<(self_type const & other) const
		{
			return ptr < other.ptr;
		}

		bool operator> (self_type const & other) const
		{
			return ptr > other.ptr;
		}

		bool operator>= (self_type const & other) const
		{
			return ptr >= other.ptr;
		}

		bool operator<= (self_type const & other) const
		{
			return ptr <= other.ptr;
		}

		self_type & operator+= (difference_type const offset)
		{
			ptr += offset;
			return *this;
		}

		self_type & operator-= (difference_type const offset)
		{
			ptr -= offset;
			return *this;
		}

		value_type & operator[] (difference_type const offset) const
		{
			return ptr[offset];
		}

	private:
		pointer ptr {nullptr};
	};

	template <typename IterT = DataT>
	struct const_iterator
	{
	public:
		using self_type = const_iterator;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = const IterT;
		using difference_type = std::ptrdiff_t;
		using pointer = IterT const *;
		using reference = IterT const &;

		explicit const_iterator (pointer ptr) :
				ptr (ptr)
		{
		}
		const_iterator (const_iterator const & other) = default;
		const_iterator & operator= (const_iterator const & other) = default;
		const_iterator & operator= (IterT const & other)
		{
			ptr = &other;
		};
		const_iterator() = default;
		~const_iterator() = default;

		self_type & operator++()
		{
			++ptr;
			return *this;
		}

		self_type operator++ (int)
		{
			self_type tmp = *this;
			++*this;
			return tmp;
		}

		bool operator== (self_type const & other) const
		{
			return ptr == other.ptre;
		}

		bool operator!= (self_type const & other) const
		{
			return ptr != other.ptr;
		}

		reference operator*() const
		{
			if (ptr == nullptr)
				throw std::bad_function_call();
			return *(ptr);
		}

		reference operator->() const
		{
			if (ptr == nullptr)
				throw std::bad_function_call();
			return *(ptr);
		}

		self_type & operator--()
		{
			--ptr;
			return *this;
		}

		self_type operator-- (int)
		{
			self_type tmp = *this;
			--*this;
			return tmp;
		}

		self_type operator+ (difference_type offset) const
		{
			self_type tmp = *this;
			return tmp += offset;
		}

		self_type operator- (difference_type offset) const
		{
			self_type tmp = *this;
			return tmp -= offset;
		}

		difference_type operator- (self_type const & other) const
		{
			return (ptr - other.ptr);
		}

		bool operator<(self_type const & other) const
		{
			return ptr < other.ptr;
		}

		bool operator> (self_type const & other) const
		{
			return ptr > other.ptr;
		}

		bool operator>= (self_type const & other) const
		{
			return ptr >= other.ptr;
		}

		bool operator<= (self_type const & other) const
		{
			return ptr <= other.ptr;
		}

		self_type & operator+= (difference_type const offset)
		{
			ptr += offset;
			return *this;
		}

		self_type & operator-= (difference_type const offset)
		{
			ptr -= offset;
			return *this;
		}

		value_type & operator[] (difference_type const offset) const
		{
			return ptr[offset];
		}

	private:
		pointer ptr {nullptr};
	};

	buffer (iterator<DataT> start, iterator<DataT> end) :
			m_size {end - start},
			m_datasize {sizeof (DataT) * m_size},
			m_buffer {(DataT *) malloc (sizeof (DataT) * m_size)}
	{
		if (this->m_buffer == nullptr)
			throw std::runtime_error ("Failed to allocate buffer space");

		std::copy (start, end, this->m_buffer);

#ifdef DEBUG
		std::cerr << __func__ << ": Copied portion of existing buffer to" << std::showbase << std::hex
							<< (std::size_t) this->m_buffer << ", size " << this->m_datasize << std::endl;
#endif
	}

	template <typename IterT>
	iterator<IterT> begin() const
	{
		return iterator<IterT> ((IterT *) this->m_buffer);
	}

	template <typename IterT>
	const_iterator<IterT> cbegin() const
	{
		return const_iterator<IterT> ((IterT *) this->m_buffer);
	}

	template <typename IterT>
	iterator<IterT> end() const
	{
		// the end iter should be divisible by the size of the iter type
		return iterator<IterT> ((IterT *) (((char *) this->m_buffer) + (m_datasize - (m_datasize % sizeof (IterT)))));
	}

	template <typename IterT>
	const_iterator<IterT> cend() const
	{
		return const_iterator<IterT> ((IterT *) (((char *) this->m_buffer) + (m_datasize - (m_datasize % sizeof (IterT)))));
	}

	iterator<DataT> begin() const
	{
		return iterator<DataT> (this->m_buffer);
	}

	const_iterator<DataT> cbegin() const
	{
		return const_iterator<DataT> (this->m_buffer);
	}

	iterator<DataT> end() const
	{
		return iterator<DataT> ((DataT *) (((char *) this->m_buffer) + m_datasize));
	}

	const_iterator<DataT> cend() const
	{
		return const_iterator<DataT> ((DataT *) (((char *) this->m_buffer) + m_datasize));
	}

	bool operator== (buffer<DataT> const & other) const
	{
		if (m_datasize != other.m_datasize)
			return false;

		if (m_buffer == other.m_buffer)
			return true;

		char * this_data = (char *) m_buffer;
		char * other_data = (char *) other.m_buffer;

		for (auto offset = 0; offset < m_datasize; ++offset)
			if (*this_data++ != *other_data++)
				return false;
		return true;
	}

	bool operator!= (buffer<DataT> const & other) const
	{
		return ! (*this == other);
	}

protected:
	static size_t const DEFAULT_BLOCK_SZ {0x20000};

	/**
	 * @brief Number of DataT elements
	 *
	 */
	size_t m_size {0};

	/**
	 * @brief Size of the buffer in bytes
	 * (i.e. sizeof(DataT) * m_size)
	 *
	 */
	size_t m_datasize {0};

	/**
	 * @brief The actual data buffer
	 *
	 */
	DataT * m_buffer {nullptr};

	void stream_in (std::istream & in, std::streamsize const block_size)
	{

		if (in.bad() || in.eof())
			throw std::runtime_error ("Input stream is in a bad state");

		char * block_buff;

		try
		{
			block_buff = (char *) malloc (block_size);
			if (block_buff == nullptr)
				throw std::runtime_error ("Failed to allocate read buffer");

			size_t bytes_read;

			while (! in.eof())
			{
				in.read (block_buff, block_size);
				if (in.bad())
					throw std::runtime_error ("Error reading data");

				bytes_read = in.gcount();

				this->m_buffer = (DataT *) realloc (this->m_buffer, (this->m_datasize + bytes_read));
				if (this->m_buffer == nullptr)
					throw std::runtime_error ("Failed to reallocate internal buffer");

				std::memcpy (((char *) this->m_buffer) + this->m_datasize, block_buff, bytes_read);

				this->m_datasize += bytes_read;
			}

			this->m_size = this->m_datasize / sizeof (DataT);

#ifdef DEBUG
			std::cerr << __func__ << ": Created buffer from stream at " << std::showbase << std::hex
								<< (std::size_t) this->m_buffer << ", size " << this->m_datasize << std::endl;
#endif
			free (block_buff);
		}
		catch (...)
		{
			free (block_buff);
			throw;
		}
	}
};

#endif
