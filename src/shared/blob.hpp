/**
 * @file blob.hpp
 * @author Damian Rogers / damian@motoi.pro
 * @copyright ©2022 Motoi Productions / Released under MIT License
 * @brief Data block wrapper with templated iterators
 *
 */

#ifndef __MOTOI__BLOB_HPP
#define __MOTOI__BLOB_HPP

#include <cstring>
#include <iostream>
#include <iterator>
#include <stdexcept>

namespace motoi
{

class blob
{
protected:
	static size_t const DEFAULT_BLOCK_SIZE {0x80000};

	/**
	 * @brief Size of the blob in bytes
	 */
	size_t m_size;

	/**
	 * @brief The actual data buffer
	 */
	void * m_buffer;

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

			size_t bytes_read {0};
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

				this->m_buffer = realloc(this->m_buffer, (this->m_size + bytes_read));
				if (this->m_buffer == nullptr)
					throw std::runtime_error("Failed to reallocate internal buffer");

				std::memcpy(((char *) this->m_buffer) + this->m_size, block_buff, bytes_read);
				this->m_size += bytes_read;
			}

#ifdef BLOB_DEBUG
			std::cerr << __func__ << ": Created buffer from stream at " << std::showbase << std::hex
								<< (std::size_t) this->m_buffer << ", size " << this->m_size << '\n'
								<< std::dec;
#endif
			free(block_buff);
		}
		catch (...)
		{
			free(block_buff);
			throw;
		}
	}

public:
	blob & operator=(blob const &) = delete;
	blob & operator=(blob &&) = delete;

	/**
	 * @brief Create an empty blob
	 */
	blob() :
			m_size {0},
			m_buffer {nullptr}
	{
	}

	/**
	 * @brief Create a blob with optional initial value
	 * @param size Size of blob in bytes
	 * @param inital Initial value for data
	 */
	explicit blob(size_t const size, char const initial = 0) :
			m_size {size},
			m_buffer {malloc(this->m_size)}
	{
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		std::memset(m_buffer, initial, m_size);

#ifdef BLOB_DEBUG
		std::cerr << __func__ << ": Created empty buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
							<< ", " << this->m_size << " bytes with initial value\n"
							<< std::dec;
#endif
	}

	/**
	 * @brief Take over existing data
	 * @param data Pointer to data
	 * @param size Size of existing data
	 */
	blob(void * data, size_t const size) :
			m_size {size},
			m_buffer {data}
	{
#ifdef BLOB_DEBUG
		std::cerr << __func__ << ": Taking over existing memory at " << std::showbase << std::hex << (std::size_t) data
							<< " to " << (std::size_t) this->m_buffer << ", " << this->m_size << " bytes\n"
							<< std::dec;
#endif
	}

	/**
	 * @brief Copy from pointer constructor
	 * @param data Pointer to data
	 * @param size Size of existing data
	 */
	blob(void const * data, size_t const size) :
			m_size {size},
			m_buffer {malloc(this->m_size)}
	{
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		std::memcpy(this->m_buffer, data, m_size);
#ifdef BLOB_DEBUG
		std::cerr << __func__ << ": Copying existing data from " << std::showbase << std::hex << (std::size_t) data
							<< " to " << (std::size_t) this->m_buffer << ", " << this->m_size << " bytes\n"
							<< std::dec;
#endif
	}

	/**
	 * @brief Copy constructor
	 */
	blob(blob const & other) :
			blob(other.m_buffer, other.m_size)
	{
	}

	/**
	 * @brief Move constructor
	 */
	blob(blob && other) noexcept :
			m_size {other.m_size},
			m_buffer {other.m_buffer}
	{
#ifdef BLOB_DEBUG
		std::cerr << __func__ << ": Moving existing buffer at " << std::showbase << std::hex << (std::size_t) other.m_buffer
							<< " to " << (std::size_t) this->m_buffer << ", " << this->m_size << " bytes\n"
							<< std::dec;
#endif
	};

	/**
	 * @brief Input stream constructor
	 * @param data Input stream
	 * @param block_size Stream read buffer (default 128KiB)
	 */
	explicit blob(std::istream & data, size_t const block_size = DEFAULT_BLOCK_SIZE)
	{
#ifdef BLOB_DEBUG
		std::cerr << __func__ << ": Reading data from input stream in " << std::showbase << std::hex
							<< (std::size_t) block_size << "byte blocks\n"
							<< std::dec;
#endif
		stream_in(data, block_size);
	}

	~blob()
	{
		if (this->m_buffer != nullptr)
		{
#ifdef BLOB_DEBUG
			std::cerr << __func__ << ": Freeing buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
								<< ", " << this->m_size << " bytes\n"
								<< std::dec;
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
	 * @brief Returns const pointer to data buffer
	 * @return DataT * const
	 */
	[[nodiscard]] void const * data() const
	{
		return this->m_buffer;
	}

	/**
	 * @brief Returns pointer to data buffer
	 * @return DataT *
	 */
	[[nodiscard]] void * data()
	{
		return this->m_buffer;
	}

	/**
	 * @brief Resize the blob
	 *
	 * @param new_size The requested new length of the blob
	 * @param initial Fill the new space with this value if the new length is
	 * greater than the old length
	 * @return size_t The new size of the blob
	 */
	size_t resize(size_t const new_size, char const initial = 0)
	{
		if (new_size == 0)
			throw std::invalid_argument("Cannot resize buffer to 0");

		if (new_size == this->m_size)
			return this->m_size;

		if (new_size > this->m_size)
			return append(new_size, initial);

#ifdef BLOB_DEBUG
		size_t prev_bytesize = this->m_size;
#endif

		this->m_buffer = realloc(this->m_buffer, new_size);
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		this->m_size = new_size;

#ifdef BLOB_DEBUG
		std::cerr << __func__ << ": Shrunk buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
							<< ", prev size " << prev_bytesize << " bytes, new size " << this->m_size << " bytes" << '\n'
							<< std::dec;
#endif

		return this->m_size;
	}

	size_t append(size_t const additional, char const initial = 0)
	{
		if (additional == 0)
			return this->m_size;

		this->m_buffer = realloc(this->m_buffer, (this->m_size + additional));
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		std::memset(reinterpret_cast<char *>(this->m_buffer) + this->m_size, initial, additional);

		this->m_size += additional;
#ifdef BLOB_DEBUG
		std::cerr << __func__ << ": Grew buffer at " << std::showbase << std::hex << (std::size_t) this->m_buffer
							<< ", new size " << this->m_size << " bytes\n"
							<< std::dec;
#endif

		return this->m_size;
	}

	size_t append(void const * const other, size_t const size)
	{
		if (size == 0)
			return this->m_size;

		this->m_buffer = realloc(this->m_buffer, this->m_size + size);
		if (this->m_buffer == nullptr)
			throw std::bad_alloc();

		std::memcpy(reinterpret_cast<char *>(this->m_buffer) + this->m_size, other, size);
		this->m_size += size;

#ifdef BLOB_DEBUG
		std::cerr << __func__ << ": Appending data from " << std::showbase << std::hex << (std::size_t) other << ", size "
							<< size << " bytes\n";
#endif

		return this->m_size;
	}

	size_t append(char const * const other, size_t size)
	{
		return append(reinterpret_cast<void const * const>(other), size);
	}

	size_t append(unsigned char const * const other, size_t size)
	{
		return append(reinterpret_cast<void const * const>(other), size);
	}

	size_t append(blob const & other)
	{
		return append(other.m_buffer, other.m_size);
	}

	operator void *()
	{
		return m_buffer;
	}

	operator char *()
	{
		return reinterpret_cast<char *>(m_buffer);
	}

	operator unsigned char *()
	{
		return reinterpret_cast<unsigned char *>(m_buffer);
	}

	operator void const *() const
	{
		return m_buffer;
	}

	operator char const *() const
	{
		return reinterpret_cast<char const *>(m_buffer);
	}

	operator unsigned char const *() const
	{
		return reinterpret_cast<unsigned char const *>(m_buffer);
	}

	friend std::ostream & operator<<(std::ostream & out, const blob & buffer)
	{
		std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<char>(out));
		return out;
	}

	friend std::istream & operator>>(std::istream & in, blob & buffer)
	{
		buffer.stream_in(in, DEFAULT_BLOCK_SIZE);
		return in;
	}

	/**
	 * @warning No range checking performed
	 */
	template <typename DataT = char>
	DataT & operator[](size_t const index)
	{
		return reinterpret_cast<DataT *>(this->m_buffer)[index];
	}

	/**
	 * @warning No range checking performed
	 */
	template <typename DataT = char>
	DataT const & operator[](size_t const index) const
	{
		return reinterpret_cast<DataT *>(this->m_buffer)[index];
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
		pointer m_ptr {nullptr};
		size_t m_stride;

	public:
		iterator() = delete;

		~iterator() = default;

		explicit iterator(void * ptr, size_t stride = 1) :
				m_ptr(reinterpret_cast<IterT *>(ptr)),
				m_stride(stride)
		{
			if (m_stride < 1)
				throw std::invalid_argument("Stride cannot be less than 1");
		}

		iterator(iterator const & other) = default;

		iterator & operator=(self_type const & other) = default;

		iterator & operator=(IterT const & other)
		{
			m_ptr = &other;
		}

		self_type & operator++()
		{
			m_ptr += m_stride;
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
			return m_ptr == other.m_ptr;
		}

		bool operator!=(self_type const & other) const
		{
			return m_ptr != other.m_ptr;
		}

		reference operator*() const
		{
			return *m_ptr;
		}

		pointer operator->() const
		{
			return *m_ptr;
		}

		self_type & operator--()
		{
			m_ptr -= m_stride;
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
			return (m_ptr - other.m_ptr) / m_stride;
		}

		bool operator<(self_type const & other) const
		{
			return m_ptr < other.m_ptr;
		}

		bool operator>(self_type const & other) const
		{
			return m_ptr > other.m_ptr;
		}

		bool operator>=(self_type const & other) const
		{
			return m_ptr >= other.m_ptr;
		}

		bool operator<=(self_type const & other) const
		{
			return m_ptr <= other.m_ptr;
		}

		self_type & operator+=(difference_type const offset)
		{
			m_ptr += (offset * m_stride);
			return *this;
		}

		self_type & operator-=(difference_type const offset)
		{
			m_ptr -= (offset * m_stride);
			return *this;
		}

		value_type & operator[](difference_type const offset) const
		{
			return m_ptr[offset * m_stride];
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
		size_t m_stride;

	public:
		const_iterator() = default;

		~const_iterator() = default;

		explicit const_iterator(pointer ptr, size_t stride = 1) :
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
	template <typename DataT>
	blob(const_iterator<DataT> start, const_iterator<DataT> end) :
			m_size {sizeof(DataT) * (end - start)},
			m_buffer {malloc(this->m_size)}
	{
		if (this->m_buffer == nullptr)
			throw std::runtime_error("Failed to allocate buffer space");

		std::copy(start, end, this->m_buffer);

#ifdef BLOB_DEBUG
		std::cerr << __func__ << ": Copied portion of existing buffer to" << std::showbase << std::hex
							<< (std::size_t) this->m_buffer << ", " << this->m_size << " bytes\n"
							<< std::dec;
#endif
	}

	template <typename IterT = char>
	[[nodiscard]] iterator<IterT> begin(size_t stride = 1) const
	{
		return iterator<IterT>((IterT *) this->m_buffer, stride);
	}

	template <typename IterT = char>
	const_iterator<IterT> cbegin(size_t stride = 1) const
	{
		return const_iterator<IterT>((IterT *) this->m_buffer, stride);
	}

	template <typename IterT = char>
	[[nodiscard]] iterator<IterT> begin_at(off_t offset, size_t stride = 1) const
	{
		return iterator<IterT>(reinterpret_cast<char *>(this->m_buffer) + offset, stride);
	}

	template <typename IterT = char>
	const_iterator<IterT> cbegin_at(off_t offset, size_t stride = 1) const
	{
		return const_iterator<IterT>(reinterpret_cast<char *>(this->m_buffer), stride);
	}

	template <typename IterT = char>
	[[nodiscard]] iterator<IterT> end(size_t stride = 1) const
	{
		// determine the end iterator by considering the requested type and block
		// size to ensure it does not exceed the allocated space
		return iterator<IterT>(
			(IterT *) (((char *) this->m_buffer) + (m_size - (m_size % (sizeof(IterT) * stride)))), stride);
	}

	template <typename IterT = char>
	const_iterator<IterT> cend(size_t stride = 1) const
	{
		return const_iterator<IterT>(
			(IterT *) (((char *) this->m_buffer) + (m_size - (m_size % (sizeof(IterT) * stride)))), stride);
	}

	bool operator==(blob const & other) const
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

	bool operator!=(blob const & other) const
	{
		return ! (*this == other);
	}

protected:
};
} // namespace motoi

#endif
