/**
 * @file buffer.hpp
 * @author Motoi Productions (Damian Rogers / damian@motoi.pro)
 * @brief Data buffer wrapper
 *
 * Updates:
 * 20220410 Initial
 * 20220420 Added templated size() method
 */
#ifndef __MOTOI__BUFFER_HPP
#define __MOTOI__BUFFER_HPP

#include <algorithm>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>

template <typename DataT = char> class buffer
{
public:
	buffer() = delete;
	buffer & operator=(buffer const &) = delete;
	buffer & operator=(buffer &&) = delete;

	/**
	 * \brief Copy constructor
	 */
	buffer(buffer const & other) :
			m_datasize { other.m_datasize },
			m_buffer { (DataT *)malloc(other.m_datasize) }, m_size { other.m_size }
	{
		if(this->m_buffer == nullptr)
			throw std::runtime_error("Could not create buffer");
		// std::copy_n(other.p_buffer, other.p_datasize, this->p_buffer);
		std::memcpy(this->m_buffer, other.m_buffer, other.m_datasize);
#ifdef DEBUG_BUFFER
		std::cerr << "Copying existing buffer at " << std::showbase << std::hex
							<< (std::size_t)other.m_buffer << " to "
							<< (std::size_t)this->m_buffer << ", datasize "
							<< this->m_datasize << ", count " << this->m_size << std::endl;
#endif
	};

	/**
	 * \brief Move constructor
	 */
	buffer(buffer && other) :
			m_datasize { other.m_datasize }, m_buffer { std::move(other.m_buffer) },
			m_size { other.m_size }
	{
		if(this->m_buffer == nullptr)
			throw std::runtime_error("Source buffer is null");
#ifdef DEBUG_BUFFER
		std::cerr << "Moving existing buffer at " << std::showbase << std::hex
							<< (std::size_t)other.m_buffer << " to "
							<< (std::size_t)this->m_buffer << ", datasize "
							<< this->m_datasize << std::endl;
#endif
	};

	/**
	 * \brief Input stream constructor
	 * \param data Input stream
	 * \param block_size Stream read buffer (default 128kKiB)
	 */
	buffer(std::istream & data, size_t const block_size = DEFAULT_BLOCK_SZ) :
			m_datasize { 0 }, m_size { 0 }, m_buffer { nullptr }
	{
#ifdef DEBUG_BUFFER
		std::cerr << "Reading data from input stream in " << std::showbase
							<< std::hex << (std::size_t)block_size << "byte blocks"
							<< std::endl;
#endif
		stream_in(data, block_size);
	}

	/**
	 * \brief Pointer constructor
	 * The pointer will be owned by the buffer class and will be freed upon
	 * destruction.
	 * \param data Pointer to data
	 * \param size Size of existing data
	 */
	buffer(DataT * data, size_t const size) :
			m_buffer { data }, m_size(size), m_datasize { sizeof(DataT) * size }
	{
#ifdef DEBUG_BUFFER
		std::cerr << __func__ << ": Taking existing buffer at " << std::showbase
							<< std::hex << (std::size_t)this->m_buffer << ", datasize "
							<< this->m_datasize << std::endl;
#endif
	}

	/**
	 * Create an empty buffer of size bytes and optional initial value
	 * \param size Number of DataT elements
	 */
	buffer(size_t const size, char const initial = 0) :
			m_size(size), m_datasize(sizeof(DataT) * size)
	{
		this->m_buffer = (DataT *)malloc(this->m_datasize);
		if(this->m_buffer == nullptr)
			throw std::runtime_error("Failed to allocate buffer space");

		std::fill(this->begin(), this->end(), initial);
#ifdef DEBUG_BUFFER
		std::cerr << __func__ << ": Created empty buffer at " << std::showbase
							<< std::hex << (std::size_t)this->m_buffer << ", datasize "
							<< this->m_datasize << std::endl;
#endif
	}

	~buffer()
	{
		if(this->m_buffer != nullptr)
		{
#ifdef DEBUG_BUFFER
			std::cerr << "Freeing buffer at " << std::showbase << std::hex
								<< (std::size_t)this->m_buffer << ", datasize "
								<< this->m_datasize << std::endl;
#endif
			free(this->m_buffer);
		}
	}

	/**
	 * @return size_t size of the buffer *in bytes*
	 */
	size_t datasize() const
	{
		return this->m_datasize;
	}

	/**
	 * @return size_t size of the buffer in type DataT
	 */
	size_t size() const
	{
		return this->m_size;
	}

	template <typename SizeT = DataT> size_t size() const
	{
		return this->m_datasize / sizeof(SizeT);
	}

	/**
	 * @return DataT const* pointer to buffer in memory
	 */
	DataT const * data() const
	{
		return this->m_buffer;
	}

	/**
	 * @return DataT const* pointer to buffer in memory
	 */
	DataT * data()
	{
		return this->m_buffer;
	}

	size_t resize(size_t const new_size)
	{
		if(new_size == 0 || new_size == this->m_size)
			return this->m_size;

		if(new_size > this->m_size)
			return append(new_size);

#ifdef DEBUG_BUFFER
		size_t prev_datasize = this->m_datasize;
#endif

		size_t new_size_bytes = new_size * sizeof(DataT);
		this->m_buffer = (DataT *)realloc(this->m_buffer, new_size_bytes);
		if(this->m_buffer == nullptr)
			throw std::bad_alloc();

		this->m_datasize = new_size_bytes;
		this->m_size = new_size;

#ifdef DEBUG_BUFFER
		std::cerr << "Shrunk buffer at " << std::showbase << std::hex
							<< (std::size_t)this->m_buffer << ", prev datasize "
							<< prev_datasize << " new datasize " << this->m_datasize
							<< std::endl;
#endif

		return this->m_size;
	}

	size_t append(size_t const size, char const initial = 0)
	{
		if(size == 0)
			return this->m_size;

		size_t additional_bytes = size * sizeof(DataT);
		this->m_buffer =
				(DataT *)realloc(this->m_buffer, (this->m_datasize + additional_bytes));
		if(this->m_buffer == nullptr)
			throw std::bad_alloc();
		std::fill_n(this->m_buffer + this->m_datasize, additional_bytes, 0);
		this->m_datasize += additional_bytes;
		this->m_size += size;
#ifdef DEBUG_BUFFER
		std::cerr << "Grew buffer at " << std::showbase << std::hex
							<< (std::size_t)this->m_buffer << ", new datasize "
							<< this->m_datasize << std::endl;
#endif

		return this->m_size;
	}

	size_t append(buffer const & other)
	{
		if(other.size() == 0)
			return this->m_size;

#ifdef DEBUG_BUFFER
		std::cerr << "Appending data from other buffer at " << std::showbase
							<< std::hex << (std::size_t)other.m_buffer << ", datasize "
							<< other.datasize << std::endl;
#endif

		this->m_buffer =
				(DataT *)realloc(this->m_buffer, this->m_datasize + other.m_datasize);
		if(this->m_buffer == nullptr)
			throw std::bad_alloc();

		// std::copy((char *)other.m_buffer,
		//					(char *)(other.m_buffer) + other.m_datasize,
		//					(char *)(this->m_buffer) + this->m_datasize);
		std::memcpy((char *)(this->m_buffer) + this->m_datasize,
								(char *)other.m_buffer, other.m_datasize);

		this->m_datasize += other.m_datasize;
		this->m_size += other.m_size;

		return this->m_size;
	}

	size_t append(DataT * const other, size_t size)
	{
		if(size == 0)
			return this->m_size;

#ifdef DEBUG_BUFFER
		std::cerr << "Appending data via pointer to " << std::showbase << std::hex
							<< (std::size_t)other << ", datasize " << size << std::endl;
#endif

		this->m_buffer = (DataT *)realloc(this->m_buffer, this->m_datasize + size);
		if(this->m_buffer == nullptr)
			throw std::bad_alloc();

		// std::copy((char *)other.m_buffer,
		//					(char *)(other.m_buffer) + other.m_datasize,
		//					(char *)(this->m_buffer) + this->m_datasize);
		std::memcpy((char *)(this->m_buffer) + this->m_datasize, (char *)other,
								size);

		this->m_datasize += size;
		this->m_size += size / sizeof(DataT);

		return this->m_size;
	}

	friend std::ostream & operator<<(std::ostream & out, const buffer & buffer)
	{
		std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<DataT>(out));
		return out;
	}

	friend std::istream & operator>>(std::istream & in, buffer & buffer)
	{
		buffer->stream_in(in, DEFAULT_BLOCK_SZ);
		return in;
	}

	DataT & operator[](size_t const index)
	{
		if(index > this->m_size)
			throw std::out_of_range("Specified subscript out of range");

		return this->m_buffer[index];
	}

	DataT const & operator[](size_t const index) const
	{
		if(index > this->m_size)
			throw std::out_of_range("Specified subscript out of range");

		return this->m_buffer[index];
	}

	// TODO: proper bounds checking on the iterator
	template <typename IterT = DataT> struct iterator
	{
	public:
		using self_type = iterator;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = IterT;
		using difference_type = std::ptrdiff_t;
		using pointer = IterT *;
		using reference = IterT &;

		explicit iterator(pointer ptr) : ptr(ptr) {}
		iterator(iterator const & other) = default;
		iterator & operator=(self_type const & other) = default;
		iterator & operator=(IterT const & other)
		{
			ptr = &other;
		};
		iterator() = default;
		~iterator() = default;

		self_type & operator++()
		{
			++ptr;
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
			return ptr == other.ptre;
		}

		bool operator!=(self_type const & other) const
		{
			return ptr != other.ptr;
		}

		reference operator*() const
		{
			if(ptr == nullptr)
				throw std::bad_function_call();
			return *(ptr);
		}

		pointer operator->() const
		{
			if(ptr == nullptr)
				throw std::bad_function_call();
			return *(ptr);
		}

		self_type & operator--()
		{
			--ptr;
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
			return tmp -= offset;
		}

		difference_type operator-(self_type const & other) const
		{
			return (ptr - other.ptr);
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
			ptr += offset;
			return *this;
		}

		self_type & operator-=(difference_type const offset)
		{
			ptr -= offset;
			return *this;
		}

		value_type & operator[](difference_type const offset) const
		{
			return ptr[offset];
		}

	private:
		pointer ptr { nullptr };
	};

	template <typename IterT = DataT> struct const_iterator
	{
	public:
		using self_type = const_iterator;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = const IterT;
		using difference_type = std::ptrdiff_t;
		using pointer = IterT const *;
		using reference = IterT const &;

		explicit const_iterator(pointer ptr) : ptr(ptr) {}
		const_iterator(const_iterator const & other) = default;
		const_iterator & operator=(const_iterator const & other) = default;
		const_iterator & operator=(IterT const & other)
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

		self_type operator++(int)
		{
			self_type tmp = *this;
			++*this;
			return tmp;
		}

		bool operator==(self_type const & other) const
		{
			return ptr == other.ptre;
		}

		bool operator!=(self_type const & other) const
		{
			return ptr != other.ptr;
		}

		reference operator*() const
		{
			if(ptr == nullptr)
				throw std::bad_function_call();
			return *(ptr);
		}

		reference operator->() const
		{
			if(ptr == nullptr)
				throw std::bad_function_call();
			return *(ptr);
		}

		self_type & operator--()
		{
			--ptr;
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
			return tmp -= offset;
		}

		difference_type operator-(self_type const & other) const
		{
			return (ptr - other.ptr);
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
			ptr += offset;
			return *this;
		}

		self_type & operator-=(difference_type const offset)
		{
			ptr -= offset;
			return *this;
		}

		value_type & operator[](difference_type const offset) const
		{
			return ptr[offset];
		}

	private:
		pointer ptr { nullptr };
	};

	buffer(iterator<DataT> start, iterator<DataT> end)
	{
		this->m_datasize = end - start;
		this->m_buffer = (DataT *)malloc(this->m_datasize);
		if(this->m_buffer == nullptr)
			throw std::runtime_error("Could not create buffer");
		std::copy(start, end, this->m_buffer);
#ifdef DEBUG_BUFFER
		std::cerr << "Copied portion of existing buffer to" << std::showbase
							<< std::hex << (std::size_t)this->m_buffer << ", size "
							<< this->m_datasize << std::endl;
#endif
	}

	template <typename IterT> iterator<IterT> begin() const
	{
		return iterator<IterT>((IterT *)this->m_buffer);
	}

	template <typename IterT> const_iterator<IterT> cbegin() const
	{
		return const_iterator<IterT>((IterT *)this->m_buffer);
	}

	template <typename IterT> iterator<IterT> end() const
	{
		// the end iter should be divisible by the size of the iter type
		return iterator<IterT>(
				(IterT *)(((char *)this->m_buffer) +
									(m_datasize - (m_datasize % sizeof(IterT)))));
	}

	template <typename IterT> const_iterator<IterT> cend() const
	{
		return const_iterator<IterT>(
				(IterT *)(((char *)this->m_buffer) + m_datasize));
	}

	iterator<DataT> begin() const
	{
		return iterator<DataT>(this->m_buffer);
	}

	const_iterator<DataT> cbegin() const
	{
		return const_iterator<DataT>(this->m_buffer);
	}

	iterator<DataT> end() const
	{
		return iterator<DataT>((DataT *)(((char *)this->m_buffer) + m_datasize));
	}

	const_iterator<DataT> cend() const
	{
		return const_iterator<DataT>(
				(DataT *)(((char *)this->m_buffer) + m_datasize));
	}

	bool operator==(buffer<DataT> const & other) const
	{
		if(m_datasize != other.m_datasize)
			return false;
		char * this_data = (char *)m_buffer;
		char * other_data = (char *)other.m_buffer;

		for(auto offset = 0; offset < m_datasize; ++offset)
			if(*this_data++ != *other_data++)
				return false;
		return true;
	}

	bool operator!=(buffer<DataT> const & other) const
	{
		return !(*this == other);
	}

protected:
	static size_t const DEFAULT_BLOCK_SZ { 0x20000 };

	DataT * m_buffer { nullptr };

	// m_datasize is the *size in bytes*
	size_t m_datasize { 0 };

	// m_size is the *size in DataT*
	size_t m_size { 0 };

	void stream_in(std::istream & in, size_t const block_size)
	{

		if(in.bad() || in.eof())
			throw std::runtime_error("Input stream is in a bad state");

		char * block_buff { nullptr };

		try
		{
			block_buff = (char *)malloc(block_size);
			if(block_buff == nullptr)
				throw std::runtime_error("Failed to allocate read buffer");

			size_t bytes_read { 0 };

			while(!in.eof())
			{
				in.read(block_buff, block_size);
				if(in.bad())
					throw std::runtime_error("Error reading data");

				bytes_read = in.gcount();

				this->m_buffer =
						(DataT *)realloc(this->m_buffer, (this->m_datasize + bytes_read));
				if(this->m_buffer == nullptr)
					throw std::runtime_error("Failed to reallocate internal buffer");

				// std::copy_n(block_buff, bytes_read,
				//						buffer.p_buffer + buffer.p_datasize);
				std::memcpy(((char *)this->m_buffer) + this->m_datasize, block_buff,
										bytes_read);

				this->m_datasize += bytes_read;
			}

			this->m_size = this->m_datasize / sizeof(DataT);

#ifdef DEBUG_BUFFER
			std::cerr << "Created buffer from stream at " << std::showbase << std::hex
								<< (std::size_t)this->m_buffer << ", size " << this->m_datasize
								<< std::endl;
#endif
			if(block_buff != nullptr)
				free(block_buff);
		}
		catch(...)
		{
			if(block_buff != nullptr)
				free(block_buff);
			throw;
		}
	}
};

#endif
