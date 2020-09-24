		#pragma once
		#include <cstdint>
		#include <assert.h>
		#include <cstring>

		#define CHECK_BIT(value, bit) ((value >> bit) & 1)
		#define BITMAP_NO_BITS_LEFT   0xFFFFFFFF

		template <size_t SIZE> class Bitmap {
		private:
			uint8_t m_bitmap_data[SIZE];

		public:
			void init();
			void set_used(unsigned position);
			void set_unused(unsigned position);
			unsigned find_unused(unsigned search_start = 0);
			unsigned find_used(unsigned search_start = 0);
			bool check_used(unsigned position);
			bool check_unused(unsigned position);
		};


		template <size_t SIZE> void Bitmap<SIZE>::init() {
			memset(m_bitmap_data, 0, sizeof(m_bitmap_data));
		}


		template <size_t SIZE> void Bitmap<SIZE>::set_used(unsigned position) {
			assert(position < SIZE);
			m_bitmap_data[position / 8] |= (1 << (position % 8));
		}

		template <size_t SIZE> void Bitmap<SIZE>::set_unused(unsigned position) {
			assert(position < SIZE);
			m_bitmap_data[position / 8] &= ~(1 << (position % 8));
		}

		template <size_t SIZE> unsigned Bitmap<SIZE>::find_unused(unsigned search_start) {
			assert(search_start < SIZE);
			size_t bit_index = search_start;
			while (bit_index < SIZE) {
				if (m_bitmap_data[bit_index / 8] == 0xFF) {
					bit_index += 8;
					continue;
				}
				if (!CHECK_BIT(m_bitmap_data[bit_index / 8], bit_index % 8))
					return bit_index;

				bit_index++;
			}
			return BITMAP_NO_BITS_LEFT;
		}

		template <size_t SIZE> unsigned Bitmap<SIZE>::find_used(unsigned search_start) {
			assert(search_start < SIZE);
			size_t bit_index = search_start;
			while (bit_index < SIZE) {
				if (m_bitmap_data[bit_index / 8] == 0) {
					bit_index += 8;
					continue;
				}
				if (CHECK_BIT(m_bitmap_data[bit_index / 8], bit_index % 8))
					return bit_index;

				bit_index++;
			}
			return BITMAP_NO_BITS_LEFT;
		}

		template <size_t SIZE> bool Bitmap<SIZE>::check_used(unsigned position) {
			return CHECK_BIT(m_bitmap_data[position / 8], position % 8);
		}

		template <size_t SIZE> bool Bitmap<SIZE>::check_unused(unsigned position) {
			 return !CHECK_BIT(m_bitmap_data[position / 8], position % 8);
		}