/**
 * @file blob.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @brief Data blob wrapper with templated iterators
 * @copyright ©2022 Motoi Productions / Released under MIT License
 *
 * Updates:
 * 20220410 Initial
 * 20220420 Added templated size() method
 * 20220921 Added vector constructor; changed pointer constructor to copy;
 * consolidated similar constructors; corrected bug with append; a number of
 * changes based on clang-tidy suggestions
 * 20221015 Fixed misrepresentation of problem when streaming input to buffer
 * and 0 bytes read  (e.g. stream in a bad state)
 * 20230305 Added iteration by stride; added namespace; differentiated blob dimension by "length" (the size of the data
 * buffer by DataT types) and "size" (the size of the data buffer in bytes); bugfixes
 * 20230416 Corrected bug where istreams with files less than the block size were not having their size set correctly;
 * changed default block size to 512kb
 */

#ifndef __MOTOI__BLOB_HPP
#define __MOTOI__BLOB_HPP

#include <cstring>
#include <iostream>
#include <iterator>
#include <stdexcept>
#include <vector>

namespace motoi
{
template <typename DataT = char>
class blob
{
public:
	// semantic typedef to differentiate length (data size in sizeof(DataT)) and size (data size in bytes)
	using length_t = size_t;

	blob & operator=(blob const &) = delete;
	blob & operator=(blob &&) = delete;

	/**
	 * @brief Create an empty blob
	 */
	blob() = default;

	/**
	 * @brief Create a blob with optional initial value
	 * @param size Number of DataT elements
	 */
	explicit blob(length_t const length, DataT const initial = static_cast<DataT>(0)) :
			m_length {length},
			m_size {sizeof(DataT) * length},
			m_buffer {(DataT *) malloc(this->m_size)}
	{
		if (this->m_buffer == nullptr)
			throw std::runtime_error("Failed to allocate buffer space");

		if (initial == static_cast<DataT>(0))
			std::memset(m_buffer, 0, m_size);
		else
		{
			auto p_begin = m_buffer;
			auto p_end = p_begin + m_length;
			while (p_begin < p_end)
				*p_begin++ = initial;
		}

#ifdef MOTOI_DEBUG
		std::cerr << __func__ << ": Created empty buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
							<< ", " << this->m_bytesize " byte" << std::endl;
#endif
	}

	/**
	 * @brief Copy from pointer constructor
	 * @param data Pointer to data
	 * @param size Size of existing data
	 */
	blob(DataT const * data, length_t const length) :
			m_length {length},
			m_size {sizeof(DataT) * length},
			m_buffer {(DataT *) malloc(this->m_size)}
	{
		if (this->m_buffer == nullptr)
			throw std::runtime_error("Failed to allocate buffer space");

		std::memcpy(this->m_buffer, data, m_size);
#ifdef MOTOI_DEBUG
		std::cerr << __func__ << ": Copying existing buffer from " << std::showbase << std::hex << (std::size_t) data
							<< " to " << (std::size_t) this->m_buffer << ", " << this->m_bytesize << " bytes" << std::endl;
#endif
	}

	/**
	 * @brief Copy constructor
	 */
	blob(blob const & other) :
			blob(other.m_buffer, other.m_length)
	{
	}

	/**
	 * @brief Move constructor
	 */
	blob(blob && other) noexcept :
			m_length {other.m_length},
			m_size {other.m_size},
			m_buffer {std::move(other.m_buffer)}
	{
#ifdef MOTOI_DEBUG
		std::cerr << __func__ << ": Moving existing buffer at " << std::showbase << std::hex << (std::size_t) other.m_buffer
							<< " to " << (std::size_t) this->m_buffer << ", " << this->m_bytesize " bytes" << std::endl;
#endif
	};

	/**
	 * @brief Input stream constructor
	 * @param data Input stream
	 * @param block_size Stream read buffer (default 128KiB)
	 */
	explicit blob(std::istream & data, size_t const block_size = DEFAULT_BLOCK_SZ)
	{
#ifdef MOTOI_DEBUG
		std::cerr << __func__ << ": Reading data from input stream in " << std::showbase << std::hex
							<< (std::size_t) block_size << "byte blocks" << std::endl;
#endif
		stream_in(data, block_size);
	}

	/**
	 * @brief Construct from vector
	 */
	blob(std::vector<DataT> & vec) :
			blob(vec.data(), vec.size())
	{
	}

	~blob()
	{
		if (this->m_buffer != nullptr)
		{
#ifdef MOTOI_DEBUG
			std::cerr << __func__ << ": Freeing buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
								<< ", " << this->m_bytesize << " bytes" << std::endl;
#endif
			free(this->m_buffer);
		}
	}

	/**
	 * @brief Returns the size of the blob in bytes
	 * @return size_t
	 */
	[[nodiscard]] size_t size() const
	{
		return this->m_size;
	}

	/**
	 * @brief Returns length of the blob by default type
	 * @return length_t
	 */
	[[nodiscard]] length_t length() const
	{
		return this->m_length;
	}

	/**
	 * @brief Returns length of the blob by specified type
	 * @return length_t
	 */
	template <typename LengthT>
	[[nodiscard]] length_t length() const
	{
		return this->m_size / sizeof(LengthT);
	}

	/**
	 * @brief Returns const pointer to buffer in memory
	 * @return DataT * const
	 */
	[[nodiscard]] DataT const * data() const
	{
		return this->m_buffer;
	}

	/**
	 * @brief Returns pointer to buffer in memory
	 * @return DataT *
	 */
	[[nodiscard]] DataT * data()
	{
		return this->m_buffer;
	}

	/**
	 * @brief Resize the blob to the specified length
	 *
	 * @param new_length The requested new length of the blob
	 * @param initial Fill the new space with this value if the new length is greater than the old length
	 * @return length_t The new length of the blob
	 */
	length_t resize(length_t const new_length, DataT initial = 0)
	{
		if (new_length == 0)
			throw std::invalid_argument("Cannot resize buffer to 0");

		if (new_length == this->m_length)
			return this->m_length;

		if (new_length > this->m_length)
			return append(new_length, initial);

#ifdef MOTOI_DEBUG
		size_t prev_bytesize = this->m_bytesize;
#endif

		size_t new_size_bytes = new_length * sizeof(DataT);
		this->m_buffer = (DataT *) realloc(this->m_buffer, new_size_bytes);
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		this->m_size = new_size_bytes;
		this->m_length = new_length;

#ifdef MOTOI_DEBUG
		std::cerr << __func__ << ": Shrunk buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
							<< ", prev size " << prev_bytesize << " bytes, new size " << this->m_bytesize << " bytes" << std::dec
							<< std::endl;
#endif

		return this->m_length;
	}

	length_t append(length_t const length, DataT const initial = 0)
	{
		if (length == 0)
			return this->m_length;

		size_t additional_bytes = length * sizeof(DataT);
		this->m_buffer = (DataT *) realloc(this->m_buffer, (this->m_size + additional_bytes));
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		if (initial == 0)
			std::memset(this->m_buffer + this->m_size, 0, additional_bytes);
		else
		{
			auto p_begin = this->m_buffer + this->m_size;
			auto p_end = p_begin + length;
			while (p_begin < p_end)
				*p_begin++ = initial;
		}

		this->m_size += additional_bytes;
		this->m_length += length;
#ifdef MOTOI_DEBUG
		std::cerr << __func__ << ": Grew buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
							<< ", new size " << this->m_bytesize << " bytes" << std::endl;
#endif

		return this->m_length;
	}

	length_t append(blob const & other)
	{
		if (other.length() == 0)
			return this->m_length;

#ifdef MOTOI_DEBUG
		std::cerr << __func__ << ": Appending data from other buffer at " << std::showbase << std::hex
							<< (std::size_t) other.m_buffer << ", size " << other.m_bytesize << " bytes" << std::endl;
#endif

		this->m_buffer = (DataT *) realloc(this->m_buffer, this->m_size + other.m_size);
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		std::memcpy((char *) (this->m_buffer) + this->m_size, (char *) other.m_buffer, other.m_size);

		this->m_size += other.m_size;
		this->m_length += other.m_length;

		return this->m_length;
	}

	length_t append(DataT * const other, length_t length)
	{
		if (length == 0)
			return this->m_length;

#ifdef MOTOI_DEBUG
		std::cerr << __func__ << ": Appending data via pointer to " << std::showbase << std::hex << (std::size_t) other
							<< ", datasize " << size << std::endl;
#endif

		this->m_buffer = (DataT *) realloc(this->m_buffer, this->m_size + length);
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		std::memcpy((char *) (this->m_buffer) + this->m_size, (char *) other, length);

		this->m_size += length;
		this->m_length += length / sizeof(DataT);

		return this->m_length;
	}

	friend std::ostream & operator<<(std::ostream & out, const blob & buffer)
	{
		std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<DataT>(out));
		return out;
	}

	friend std::istream & operator>>(std::istream & in, blob & buffer)
	{
		buffer->stream_in(in, DEFAULT_BLOCK_SZ);
		return in;
	}

	DataT & operator[](length_t const index)
	{
		if (index > this->m_length)
			throw std::out_of_range("Specified subscript out of range");

		return this->m_buffer[index];
	}

	DataT const & operator[](length_t const index) const
	{
		if (index > this->m_length)
			throw std::out_of_range("Specified subscript out of range");

		return this->m_buffer[index];
	}

	template <typename IterT>
	struct iterator
	{
	public:
		using self_type = iterator;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = IterT;
		using difference_type = std::ptrdiff_t;
		using pointer = IterT *;
		using reference = IterT &;

	protected:
		pointer ptr {nullptr};
		length_t m_stride;

	public:
		iterator() = delete;

		~iterator() = default;

		explicit iterator(pointer ptr, size_t stride = 1) :
				ptr(ptr),
				m_stride(stride)
		{
			if (m_stride < 1)
				throw std::invalid_argument("Stride cannot be less than 1");
		}

		iterator(iterator const & other) = default;

		iterator & operator=(self_type const & other) = default;

		iterator & operator=(IterT const & other)
		{
			ptr = &other;
		}

		self_type & operator++()
		{
			ptr += m_stride;
			return *this;
		}

		self_type operator++(int)
		{
			self_type tmp = *this;
			++*this;
			return tmp;
		}

		bool operator==(self_type const & other) const
		{
			return ptr == other.ptr;
		}

		bool operator!=(self_type const & other) const
		{
			return ptr != other.ptr;
		}

		reference operator*() const
		{
			return *ptr;
		}

		pointer operator->() const
		{
			return *ptr;
		}

		self_type & operator--()
		{
			ptr -= m_stride;
			return *this;
		}

		self_type operator--(int)
		{
			self_type tmp = *this;
			--*this;
			return tmp;
		}

		self_type operator+(difference_type offset) const
		{
			self_type tmp = *this;
			return tmp += offset;
		}

		self_type operator-(difference_type offset) const
		{
			self_type tmp = *this;
			return (tmp -= offset) / m_stride;
		}

		difference_type operator-(self_type const & other) const
		{
			return (ptr - other.ptr) / m_stride;
		}

		bool operator<(self_type const & other) const
		{
			return ptr < other.ptr;
		}

		bool operator>(self_type const & other) const
		{
			return ptr > other.ptr;
		}

		bool operator>=(self_type const & other) const
		{
			return ptr >= other.ptr;
		}

		bool operator<=(self_type const & other) const
		{
			return ptr <= other.ptr;
		}

		self_type & operator+=(difference_type const offset)
		{
			ptr += (offset * m_stride);
			return *this;
		}

		self_type & operator-=(difference_type const offset)
		{
			ptr -= (offset * m_stride);
			return *this;
		}

		value_type & operator[](difference_type const offset) const
		{
			return ptr[offset * m_stride];
		}
	};

	template <typename IterT>
	struct const_iterator
	{
	public:
		using self_type = const_iterator;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = const IterT;
		using difference_type = std::ptrdiff_t;
		using pointer = IterT const *;
		using reference = IterT const &;

	protected:
		pointer ptr {nullptr};
		length_t m_stride;

	public:
		const_iterator() = default;

		~const_iterator() = default;

		explicit const_iterator(pointer ptr, length_t stride = 1) :
				ptr(ptr),
				m_stride(stride)
		{
			if (m_stride < 1)
				throw std::invalid_argument("Stride cannot be less than 1");
		}

		const_iterator(const_iterator const & other) = default;

		const_iterator & operator=(const_iterator const & other) = default;

		const_iterator & operator=(IterT const & other)
		{
			ptr = &other;
		};

		self_type & operator++()
		{
			ptr += m_stride;
			return *this;
		}

		self_type operator++(int)
		{
			self_type tmp = *this;
			++*this;
			return tmp;
		}

		bool operator==(self_type const & other) const
		{
			return ptr == other.ptr;
		}

		bool operator!=(self_type const & other) const
		{
			return ptr != other.ptr;
		}

		reference operator*() const
		{
			return *ptr;
		}

		reference operator->() const
		{
			return *ptr;
		}

		self_type & operator--()
		{
			ptr -= m_stride;
			return *this;
		}

		self_type operator--(int)
		{
			self_type tmp = *this;
			--*this;
			return tmp;
		}

		self_type operator+(difference_type offset) const
		{
			self_type tmp = *this;
			return tmp += offset;
		}

		self_type operator-(difference_type offset) const
		{
			self_type tmp = *this;
			return (tmp -= offset) / m_stride;
		}

		difference_type operator-(self_type const & other) const
		{
			return (ptr - other.ptr) / m_stride;
		}

		bool operator<(self_type const & other) const
		{
			return ptr < other.ptr;
		}

		bool operator>(self_type const & other) const
		{
			return ptr > other.ptr;
		}

		bool operator>=(self_type const & other) const
		{
			return ptr >= other.ptr;
		}

		bool operator<=(self_type const & other) const
		{
			return ptr <= other.ptr;
		}

		self_type & operator+=(difference_type const offset)
		{
			ptr += (offset * m_stride);
			return *this;
		}

		self_type & operator-=(difference_type const offset)
		{
			ptr -= (offset * m_stride);
			return *this;
		}

		value_type & operator[](difference_type const offset) const
		{
			return ptr[offset * m_stride];
		}
	};

	blob(const_iterator<DataT> start, const_iterator<DataT> end) :
			m_length {end - start},
			m_size {sizeof(DataT) * m_length},
			m_buffer {(DataT *) malloc(this->m_size)}
	{
		if (this->m_buffer == nullptr)
			throw std::runtime_error("Failed to allocate buffer space");

		std::copy(start, end, this->m_buffer);

#ifdef MOTOI_DEBUG
		std::cerr << __func__ << ": Copied portion of existing buffer to" << std::showbase << std::hex
							<< (std::size_t) this->m_buffer << ", size " << this->m_bytesize << std::endl;
#endif
	}

	template <typename IterT>
	iterator<IterT> begin(length_t stride = 1) const
	{
		return iterator<IterT>((IterT *) this->m_buffer, stride);
	}

	template <typename IterT>
	const_iterator<IterT> cbegin(length_t stride = 1) const
	{
		return const_iterator<IterT>((IterT *) this->m_buffer, stride);
	}

	template <typename IterT>
	iterator<IterT> end(length_t stride = 1) const
	{
		// determine the end iterator by considering the requested type and block size to ensure
		// it does not exceed the allocated space
		return iterator<IterT>(
			(IterT *) (((char *) this->m_buffer) + (m_size - (m_size % (sizeof(IterT) * stride)))), stride);
	}

	template <typename IterT>
	const_iterator<IterT> cend(length_t stride = 1) const
	{
		return const_iterator<IterT>(
			(IterT *) (((char *) this->m_buffer) + (m_size - (m_size % (sizeof(IterT) * stride)))), stride);
	}

	iterator<DataT> begin(length_t stride = 1) const
	{
		return iterator<DataT>(this->m_buffer, stride);
	}

	const_iterator<DataT> cbegin(length_t stride = 1) const
	{
		return const_iterator<DataT>(this->m_buffer, stride);
	}

	iterator<DataT> end(length_t stride = 1) const
	{
		return iterator<DataT>((DataT *) (((char *) this->m_buffer) + m_size), stride);
	}

	const_iterator<DataT> cend(length_t stride = 1) const
	{
		return const_iterator<DataT>((DataT *) (((char *) this->m_buffer) + m_size), stride);
	}

	bool operator==(blob<DataT> const & other) const
	{
		if (m_size != other.m_size)
			return false;

		if (m_buffer == other.m_buffer)
			return true;

		char * this_data = (char *) m_buffer;
		char * other_data = (char *) other.m_buffer;

		for (auto offset = 0; offset < m_size; ++offset)
			if (*this_data++ != *other_data++)
				return false;
		return true;
	}

	bool operator!=(blob<DataT> const & other) const
	{
		return ! (*this == other);
	}

protected:
	static size_t const DEFAULT_BLOCK_SZ {0x80000};

	/**
	 * @brief Length of the blob by type
	 */
	length_t m_length {0};

	/**
	 * @brief Size of the blob in bytes
	 * (i.e. sizeof(DataT) * m_size)
	 */
	size_t m_size {0};

	/**
	 * @brief The actual data buffer
	 */
	DataT * m_buffer {nullptr};

	void stream_in(std::istream & in, std::streamsize const block_size)
	{

		if (in.bad() || in.eof())
			throw std::runtime_error("Input stream is in a bad state");

		char * block_buff;

		try
		{
			block_buff = (char *) malloc(block_size);
			if (block_buff == nullptr)
				throw std::runtime_error("Failed to allocate read buffer");

			size_t bytes_read;

			while (true)
			{
				in.read(block_buff, block_size);
				if (in.bad())
					throw std::runtime_error("Error reading data");

				bytes_read = in.gcount();
				if (bytes_read == 0)
				{
					if (in.eof())
						break;
					else
						throw std::runtime_error("Failed to read data from input stream");
				}

				this->m_buffer = (DataT *) realloc(this->m_buffer, (this->m_size + bytes_read));
				if (this->m_buffer == nullptr)
					throw std::runtime_error("Failed to reallocate internal buffer");

				std::memcpy(((char *) this->m_buffer) + this->m_size, block_buff, bytes_read);
				this->m_size += bytes_read;
			}

			this->m_length = this->m_size / sizeof(DataT);

#ifdef MOTOI_DEBUG
			std::cerr << __func__ << ": Created buffer from stream at " << std::showbase << std::hex
								<< (std::size_t) this->m_buffer << ", size " << this->m_bytesize << std::endl;
#endif
			free(block_buff);
		}
		catch (...)
		{
			free(block_buff);
			throw;
		}
	}
};
} // namespace motoi

#endif
