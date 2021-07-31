#ifndef __BUFFER_HPP
#define __BUFFER_HPP

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <memory>

/**
 * Data buffer class
 */
class buffer
{
public:
	buffer() = delete;
	buffer & operator=(buffer const &) = delete;
	// buffer(buffer &&) = delete;
	buffer & operator=(buffer &&) = delete;

	/**
	 * Copy constructor
	 */
	buffer(buffer const & other) :
			p_size { other.size() }, p_buffer { (char *)malloc(this->p_size) }
	{
		if(this->p_buffer == nullptr)
			throw std::runtime_error("Could not create buffer");
		std::copy_n(other.p_buffer, other.p_size, this->p_buffer);
#ifdef DEBUG
		std::cerr << "Copying existing buffer at " << std::showbase << std::hex
							<< (std::size_t)other.p_buffer << " to "
							<< (std::size_t)this->p_buffer << ", size " << this->p_size
							<< std::endl;
#endif
	};

	/**
	 * Move constructor
	 */
	buffer(buffer && other) :
			p_size { other.size() }, p_buffer { std::move(other.p_buffer) }
	{
		if(this->p_buffer == nullptr)
			throw std::runtime_error("Source buffer is null");
#ifdef DEBUG
		std::cerr << "Moving existing buffer at " << std::showbase << std::hex
							<< (std::size_t)other.p_buffer << " to "
							<< (std::size_t)this->p_buffer << ", size " << this->p_size
							<< std::endl;
#endif
	};

	/**
	 * Read a stream of data into the buffer
	 * \param data Input stream
	 * \param block_size Stream read buffer (default 128kKiB)
	 */
	buffer(std::istream & data, size_t const block_size = DEFAULT_BLOCK_SZ) :
			p_size { 0 }, p_buffer { nullptr }
	{
		stream_in(*this, data, block_size);
	}

	/**
	 * Takes a pointer to an existing buffer of data and its size. The pointer
	 * will be owned by the buffer class and will be freed upon destruction.
	 * \param data Pointer to data
	 * \param size Size of existing data
	 */
	buffer(char * data, size_t const size) :
			p_buffer { std::move(data) }, p_size { size }
	{
#ifdef DEBUG
		std::cerr << "Taking existing buffer at " << std::showbase << std::hex
							<< (std::size_t)this->p_buffer << ", size " << this->p_size
							<< std::endl;
#endif
	}

	/**
	 * Create an empty buffer of size bytes and optional initial value
	 * \param size Size of the buffer in bytes
	 */
	buffer(size_t const size, char const initial = 0) : p_size(size)
	{
		this->p_buffer = (char *)malloc(this->p_size);
		if(this->p_buffer == nullptr)
			throw std::runtime_error("Failed to allocate buffer space");

		std::fill(this->begin(), this->end(), initial);
#ifdef DEBUG
		std::cerr << "Created empty buffer at " << std::showbase << std::hex
							<< (std::size_t)this->p_buffer << ", size " << this->p_size
							<< std::endl;
#endif
	}

	~buffer()
	{
		if(this->p_buffer != nullptr)
		{
#ifdef DEBUG
			std::cerr << "Freeing buffer at " << std::showbase << std::hex
								<< (std::size_t)this->p_buffer << ", size " << this->p_size
								<< std::endl;
#endif
			free(this->p_buffer);
		}
	}

	size_t size() const
	{
		return this->p_size;
	}

	char const * data() const
	{
		return this->p_buffer;
	}

	char * data()
	{
		return this->p_buffer;
	}

	size_t grow(size_t additional)
	{
		if(additional == 0)
			goto done;

		this->p_buffer =
				(char *)realloc(this->p_buffer, (this->p_size + additional));
		if(this->p_buffer == nullptr)
			throw std::bad_alloc();
		std::fill_n(this->p_buffer + this->p_size, additional, 0);
		this->p_size += additional;
#ifdef DEBUG
		std::cerr << "Resized buffer at " << std::showbase << std::hex
							<< (std::size_t)this->p_buffer << ", new size " << this->p_size
							<< std::endl;
#endif

	done:
		return this->p_size;
	}

	size_t append(char * data, size_t size)
	{
		if(size == 0)
			goto done;
		this->p_buffer = (char *)realloc(this->p_buffer, (this->p_size + size));
		if(this->p_buffer == nullptr)
			throw std::bad_alloc();

		std::copy(data, data + size, this->p_buffer + this->p_size);

		this->p_size += size;

	done:
		return this->p_size;
	}

	size_t append(buffer const & other)
	{
		return append(other.p_buffer, other.p_size);
	}

	friend std::ostream & operator<<(std::ostream & out, const buffer & buffer)
	{
		std::copy(buffer.begin(), buffer.end(), std::ostream_iterator<char>(out));
		return out;
	}

	friend std::istream & operator>>(std::istream & in, buffer & buffer)
	{
		stream_in(buffer, in, DEFAULT_BLOCK_SZ);
		return in;
	}

	char & operator[](size_t const index)
	{
		if(index > this->p_size)
			throw std::out_of_range("Specified subscript out of range");

		return this->p_buffer[index];
	}

	char const & operator[](size_t const index) const
	{
		if(index > this->p_size)
			throw std::out_of_range("Specified subscript out of range");

		return this->p_buffer[index];
	}

	// TODO: proper bounds checking on the iterator
	template <typename IterT> struct buffer_iterator
	{
	public:
		using self_type = buffer_iterator;
		using iterator_category = std::random_access_iterator_tag;
		using value_type = IterT;
		using difference_type = std::ptrdiff_t;
		using pointer = IterT *;
		using reference = IterT &;

		explicit buffer_iterator(pointer ptr) : ptr(ptr) {}
		buffer_iterator(buffer_iterator const & other) = default;
		buffer_iterator & operator=(buffer_iterator const & other) = default;
		buffer_iterator & operator=(char const & other)
		{
			ptr = &other;
		};
		buffer_iterator() = default;
		~buffer_iterator() = default;

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

		// TODO: make equality operator check content rather than pointer
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

		value_type & operator[](difference_type const offset)
		{
			return ptr[offset];
		}

		value_type const & operator[](difference_type const offset) const
		{
			return ptr[offset];
		}

	private:
		pointer ptr { nullptr };
	};

	typedef buffer_iterator<char> iterator;
	typedef buffer_iterator<char const> const_iterator;

	buffer(buffer::const_iterator start, buffer::const_iterator end)
	{
		this->p_size = end - start;
		this->p_buffer = (char *)malloc(this->p_size);
		if(this->p_buffer == nullptr)
			throw std::runtime_error("Could not create buffer");
		std::copy(start, end, this->p_buffer);
#ifdef DEBUG
		std::cerr << "Copied portion of existing buffer to" << std::showbase
							<< std::hex << (std::size_t)this->p_buffer << ", size "
							<< this->p_size << std::endl;
#endif
	}

	iterator begin()
	{
		return iterator(&this->p_buffer[0]);
	}

	iterator end()
	{
		return iterator(&this->p_buffer[p_size]);
	}

	const_iterator begin() const
	{
		return const_iterator(&this->p_buffer[0]);
	}

	const_iterator end() const
	{
		return const_iterator(&this->p_buffer[p_size]);
	}

protected:
	static size_t const DEFAULT_BLOCK_SZ { 0x20000 };

	char * p_buffer { nullptr };
	size_t p_size { 0 };

	static void stream_in(buffer & buffer, std::istream & in,
												size_t const block_size)
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

				buffer.p_buffer =
						(char *)realloc(buffer.p_buffer, (buffer.p_size + bytes_read));
				if(buffer.p_buffer == nullptr)
					throw std::runtime_error("Failed to reallocate internal buffer");

				std::copy_n(block_buff, bytes_read, buffer.p_buffer + buffer.p_size);

				buffer.p_size += bytes_read;
			}

#ifdef DEBUG
			std::cerr << "Created buffer from stream at " << std::showbase << std::hex
								<< (std::size_t)buffer.p_buffer << ", size " << buffer.p_size
								<< std::endl;
#endif
		}
		catch(std::exception ex)
		{
			if(block_buff != nullptr)
				free(block_buff);
			throw ex;
		}
	}
};

#endif
