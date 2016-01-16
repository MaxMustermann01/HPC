#ifndef _BUFFERS_H
#define _BUFFERS_H

/**
 * The buffers for the data to interchange.
 * For one MPI process interchanging data with its neigbours an object of this class stores the pointers to the data buffers, one for each neighbouring MPI-process.
 * 
 * @tparam SIZE an integer value representing the desired number of buffers
 */
template<int SIZE>
class Buffers {
private:
    char *m_buffers[SIZE];		/**< array of pointers to the buffers */
    int m_size[SIZE];			/**< array of buffer sizes (in bytes) */
    MPI_Datatype m_datatype;		/**< datatype of the data to store */
    
public:
  /**
   * Constructor for the buffers.
   * Initializes all pointers and sizes with standard values
   */
    explicit Buffers() {
      /* set all array values to zero */
      std::fill(&m_buffers[0], &m_buffers[SIZE - 1], nullptr);
      std::fill(&m_size[0], &m_size[SIZE - 1], 0);
    }
    
    /** 
     * Return the associated buffer for given relative coordinates in three dimensions
     * 
     * @param x relative x-coordinate of type integer
     * @param y relative y-coordinate of type integer
     * @param z relative z-coordinate of type integer
     * @return reference of a pointer to a char (i.e. the specific buffer) 
     */
    char*& buffer(int x, int y, int z) { 
      if (x < -1 || y < -1 || z < -1 || x > 1 || y > 1 || z > 1) {
	std::cout << "Wrong index: must be -1 <= i <= 1!" << std::endl;
      }
      return m_buffers[ (x+1)*9 + (y+1)*3 + z+1 ]; 
    }
    
    /** 
     * Return the associated buffer for given relative coordinates in two dimensions
     * 
     * @param x relative x-coordinate of type integer
     * @param y relative y-coordinate of type integer
     * @return reference of a pointer to a char (i.e. the specific buffer) 
     */      
    char*& buffer(int x, int y) {
      if (x<-1 || y<-1|| x>1 || y>1) {
	std::cout << "Wrong index: must be -1 <= i <= 1!" << std::endl;
      }
      return m_buffers[(x+1)*3 + y+1 ];
      
    }
    
    /** 
     * Returns reference to the used MPI datatype 
     * 
     * @return m_dataype a reference to an MPI_Datatype
     */
    MPI_Datatype& datatype(void) { 
      return m_datatype; 
    }
    
    /** 
     * Returns a reference to the size of the associated buffer for given relative coordinates in three dimensions
     * 
     * @param x relative x-coordinate of type integer
     * @param y relative y-coordinate of type integer
     * @param z relative z-coordinate of type integer
     * @return reference of an integer, the respective buffer size 
     */
    int& size(int x, int y, int z) { 
      if (x < -1 || y < -1 || z < -1 || x > 1 || y > 1 || z > 1) {
	std::cout << "Wrong index: must be -1 <= i <= 1!" << std::endl;
      } 
      return m_size[ (x+1)*9 + (y+1)*3 + z+1 ];
      
    }
    
    /** 
     * Returns a reference to the size of the associated buffer for given relative coordinates in two dimensions
     * 
     * @param x relative x-coordinate of type integer
     * @param y relative y-coordinate of type integer
     * @return reference of an integer, the respective buffer size 
     */
    int& size(int x, int y) { 
      if (x < -1 || y < -1 || x > 1 || y > 1) {
	std::cout << "Wrong index: must be -1 <= i <= 1!" << std::endl;
      } 
      return m_size[ (x+1)*3 + y+1 ]; 
      
    }
    
  };


#endif