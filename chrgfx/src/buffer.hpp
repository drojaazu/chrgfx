#ifndef __MOTOI__BUFFER_HPP
#define __MOTOI__BUFFER_HPP

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>

/**
 * Data buffer class
 * \note The template type specifier is only for
 */
template <typename DataT> class basic_buffer
{
public:
	basic_buffer() = delete;
	basic_buffer & operator=(basic_buffer const &) = delete;
	// buffer(buffer &&) = delete;
	basic_buffer & operator=(basic_buffer &&) = delete;

	/**
	 * \brief Copy constructor
	 */
	basic_buffer(basic_buffer const & other) :
			m_datasize { other.m_datasize },
			m_buffer { (DataT *)malloc(other.m_size) }, m_size { other.m_size }
	{
		if(this->m_buffer == nullptr)
			throw std::runtime_error("Could not create buffer");
		// std::copy_n(other.p_buffer, other.p_datasize, this->p_buffer);
		std::memcpy(this->m_buffer, other.m_buffer, other.m_datasize);
#ifdef DEBUG
		std::cerr << "Copying existing buffer at " << std::showbase << std::hex
							<< (std::size_t)other.m_buffer << " to "
							<< (std::size_t)this->m_buffer << ", datasize "
							<< this->m_datasize << ", count " << this->m_size << std::endl;
#endif
	};

	/**
	 * \brief Move constructor
	 */
	basic_buffer(basic_buffer && other) :
			m_datasize { other.m_datasize }, m_buffer { std::move(other.m_buffer) },
			m_size { other.m_size }
	{
		if(this->m_buffer == nullptr)
			throw std::runtime_error("Source buffer is null");
#ifdef DEBUG
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
	basic_buffer(std::istream & data,
							 size_t const block_size = DEFAULT_BLOCK_SZ) :
			m_datasize { 0 },
			m_size { 0 }, m_buffer { nullptr }
	{
		stream_in(data, block_size);
	}

	/**
	 * \brief Pointer constructor
	 * The pointer will be owned by the buffer class and will be freed upon
	 * destruction.
	 * \param data Pointer to data
	 * \param size Size of existing data
	 */
	basic_buffer(DataT * data, size_t const size) :
			m_buffer { data }, m_size(size), m_datasize { sizeof(DataT) * size }
	{
#ifdef DEBUG
		std::cerr << __func__ << ": Taking existing buffer at " << std::showbase
							<< std::hex << (std::size_t)this->m_buffer << ", datasize "
							<< this->m_datasize << std::endl;
#endif
	}

	/**
	 * Create an empty buffer of size bytes and optional initial value
	 * \param size Number of DataT elements
	 */
	basic_buffer(size_t const size, char const initial = 0) :
			m_size(size), m_datasize(sizeof(DataT) * size)
	{
		this->m_buffer = (DataT *)malloc(this->m_datasize);
		if(this->m_buffer == nullptr)
			throw std::runtime_error("Failed to allocate buffer space");

		std::fill(this->begin(), this->end(), initial);
#ifdef DEBUG
		std::cerr << __func__ << ": Created empty buffer at " << std::showbase
							<< std::hex << (std::size_t)this->m_buffer << ", datasize "
							<< this->m_datasize << std::endl;
#endif
	}

	~basic_buffer()
	{
		if(this->m_buffer != nullptr)
		{
#ifdef DEBUG
			std::cerr << "Freeing buffer at " << std::showbase << std::hex
								<< (std::size_t)this->m_buffer << ", datasize "
								<< this->m_datasize << std::endl;
#endif
			free(this->m_buffer);
		}
	}

	size_t datasize() const
	{
		return this->m_datasize;
	}

	size_t size() const
	{
		return this->m_size;
	}

	DataT const * data() const
	{
		return this->m_buffer;
	}

	DataT * data()
	{
		return this->m_buffer;
	}

	size_t grow(size_t additional)
	{
		if(additional == 0)
			return this->m_size;

		size_t additional_bytes = additional * sizeof(DataT);

		this->m_buffer =
				(DataT *)realloc(this->m_buffer, (this->m_datasize + additional_bytes));
		if(this->m_buffer == nullptr)
			throw std::bad_alloc();
		std::fill_n(this->m_buffer + this->m_datasize, additional_bytes, 0);
		this->m_datasize += additional_bytes;
		this->m_size += additional;
#ifdef DEBUG
		std::cerr << "Resized buffer at " << std::showbase << std::hex
							<< (std::size_t)this->m_buffer << ", new datasize "
							<< this->m_datasize << std::endl;
#endif

		return this->m_size;
	}

	size_t append(DataT * other_data, size_t other_size)
	{
		if(other_size == 0)
			return this->m_datasize;

		size_t other_size_bytes = other_size * sizeof(DataT);

		this->m_buffer =
				(DataT *)realloc(this->m_buffer, (this->m_datasize + other_size_bytes));
		if(this->m_buffer == nullptr)
			throw std::bad_alloc();

		// std::copy(data, data + size, this->p_buffer + this->p_size);
		std::memcpy(((char *)this->m_buffer) + this->m_datasize, (char *)other_data,
								other_size_bytes);

		this->m_datasize += other_size_bytes;
		this->m_size += other_size;

		return this->m_size;
	}

	size_t append(basic_buffer const & other)
	{
		return append(other.m_buffer, other.m_size);
	}

	friend std::ostream & operator<<(std::ostream & out,
																	 const basic_buffer & buffer)
	{
		std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<DataT>(out));
		return out;
	}

	friend std::istream & operator>>(std::istream & in, basic_buffer & buffer)
	{
		stream_in(buffer, in, DEFAULT_BLOCK_SZ);
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

	basic_buffer(iterator<DataT> start, iterator<DataT> end)
	{
		this->m_datasize = end - start;
		this->m_buffer = (DataT *)malloc(this->m_datasize);
		if(this->m_buffer == nullptr)
			throw std::runtime_error("Could not create buffer");
		std::copy(start, end, this->m_buffer);
#ifdef DEBUG
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
		return iterator<IterT>((IterT *)(((char *)this->m_buffer) + m_datasize));
	}

	template <typename IterT> const_iterator<IterT> cend() const
	{
		return const_iterator<IterT>(
				(IterT *)(((char *)this->m_buffer) + m_datasize));
	}

	iterator<DataT> begin() const
	{
		return iterator(this->m_buffer);
	}

	const_iterator<DataT> cbegin() const
	{
		return const_iterator(this->m_buffer);
	}

	iterator<DataT> end() const
	{
		return iterator((DataT *)(((char *)this->m_buffer) + m_datasize));
	}

	const_iterator<DataT> cend() const
	{
		return const_iterator((DataT *)(((char *)this->m_buffer) + m_datasize));
	}

	bool operator==(basic_buffer<DataT> const & other) const
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

	bool operator!=(basic_buffer<DataT> const & other) const
	{
		return !(*this == other);
	}

protected:
	static size_t const DEFAULT_BLOCK_SZ { 0x20000 };

	DataT * m_buffer { nullptr };
	size_t m_datasize { 0 };
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

#ifdef DEBUG
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

class buffer : public basic_buffer<char>
{
public:
	buffer(basic_buffer const & other) : basic_buffer<char>(other) {};

	buffer(basic_buffer && other) : basic_buffer<char>(other) {};

	buffer(std::istream & data, size_t const block_size = DEFAULT_BLOCK_SZ) :
			basic_buffer<char>(data, block_size) {};

	buffer(char * data, size_t const size) : basic_buffer<char>(data, size) {};

	buffer(size_t const size, char const initial = 0) :
			basic_buffer(size, initial) {};

	buffer(iterator<char> start, iterator<char> end) :
			basic_buffer(start, end) {};
};

#endif
