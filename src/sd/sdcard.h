/**
 * MK4duo Firmware for 3D Printer, Laser and CNC
 *
 * Based on Marlin, Sprinter and grbl
 * Copyright (c) 2011 Camiel Gubbels / Erik van der Zalm
 * Copyright (c) 2020 Alberto Cotronei @MagoKimbra
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 */
#pragma once

#if HAS_SD_SUPPORT

#include <SdFat.h>

#if SD_FAT_VERSION < 10101
  #error "Update SDFAT library to 1.1.1 or newer."
#endif

#define SD_SCK_DIVISOR(divisor) SPISettings(F_CPU/divisor, MSBFIRST, SPI_MODE0)

  struct sdFlags{
    bool  Mounted = false;
    bool  Printing = false;
    bool  PrintComplete = false;
    bool  Autoreport = false;
    bool  FilenameIsDir = false;
    bool  WorkdirIsRoot = false;
  };

class SDCard {

  public: /** Public Parameters */

    SdFat      fat;
    SdFile     gcode_file,
               root,
               workDir,
               workDirParents[SD_MAX_FOLDER_DEPTH];

    int autostart_index;

    uint32_t fileSize,
             sdpos;

    float  objectHeight,
           firstlayerHeight,
           layerHeight,
           filamentNeeded;

    char fileName[LONG_FILENAME_LENGTH*SD_MAX_FOLDER_DEPTH+SD_MAX_FOLDER_DEPTH+1],
         tempLongFilename[LONG_FILENAME_LENGTH+1],
          generatedBy[GENBY_SIZE];
                
    PrintFileInfo fileInfo;
    
    uint16_t fileModifiedDate;
    uint16_t fileModifiedTime;

  private: /** Private Parameters */

    sdFlags    flags;

    int8_t csPin; //CS pin
    uint8_t spiDivisor; //SPI frequency divisor
    int8_t detectPin; //SPI frequency divisor

    uint16_t nrFile_index;

    #if HAS_EEPROM_SD
      #define EEPROM_FILE_NAME "eeprom.bin"
      static eeprom_file;
    #endif

    uint16_t     workDirDepth,
                 nrFiles;          // counter for the files in the current directory and recycled as position counter for getting the nrFiles'th name in the directory.
    LsActionEnum lsAction;         // stored for recursion.

    uint8_t prev_sd_status;

    // Sort files and folders alphabetically.
    #if ENABLED(SDCARD_SORT_ALPHA)
      uint16_t sort_count;         // Count of sorted items in the current directory
      #if ENABLED(SDSORT_GCODE)
        bool sort_alpha = true;           // Flag to enable / disable the feature
        int sort_folders = FOLDER_SORTING;    // Flag to enable / disable folder sorting
        //bool sort_reverse;       // Flag to enable / disable reverse sorting
      #endif

      // By default the sort index is static
      #if ENABLED(SDSORT_DYNAMIC_RAM)
        uint8_t *sort_order;
      #else
        uint8_t sort_order[SDSORT_LIMIT];
      #endif

      #if ENABLED(SDSORT_USES_RAM) && ENABLED(SDSORT_CACHE_NAMES) && DISABLED(SDSORT_DYNAMIC_RAM)
        #define SORTED_LONGNAME_MAXLEN ((SDSORT_CACHE_VFATS) * (FILENAME_LENGTH) + 1)
      #else
        #define SORTED_LONGNAME_MAXLEN LONG_FILENAME_LENGTH
      #endif

      // Cache filenames to speed up SD menus.
      #if ENABLED(SDSORT_USES_RAM)

        // If using dynamic ram for names, allocate on the heap.
        #if ENABLED(SDSORT_CACHE_NAMES)
          #if ENABLED(SDSORT_DYNAMIC_RAM)
            char **sortshort, **sortnames;
          #else
            char sortnames[SDSORT_LIMIT][SORTED_LONGNAME_MAXLEN];
          #endif
        #elif DISABLED(SDSORT_USES_STACK)
          char sortnames[SDSORT_LIMIT][SORTED_LONGNAME_MAXLEN];
        #endif

        // Folder sorting uses an isDir array when caching items.
        #if HAS_FOLDER_SORTING
          #if ENABLED(SDSORT_DYNAMIC_RAM)
            uint8_t *isDir;
          #elif ENABLED(SDSORT_CACHE_NAMES) || DISABLED(SDSORT_USES_STACK)
            uint8_t isDir[(SDSORT_LIMIT + 7)>>3];
          #endif
        #endif

      #endif // SDSORT_USES_RAM

    #endif // SDCARD_SORT_ALPHA

    #if ENABLED(ADVANCED_SD_COMMAND)

      Sd2Card  sd;

      uint32_t cardSizeBlocks,
               cardCapacityMB;

      // Cache for SD block
      cache_t  cache;

      // MBR information
      uint8_t  partType;
      uint32_t relSector,
               partSize;

      // Fake disk geometry
      uint8_t  numberOfHeads,
               sectorsPerTrack;

      // FAT parameters
      uint16_t reservedSectors;
      uint8_t  sectorsPerCluster;
      uint32_t fatStart,
               fatSize,
               dataStart;

      // constants for file system structure
      static uint16_t const BU16 = 128,
                     	 	BU32 = 8192;

      static const uint32_t ERASE_SIZE = 262144L;

    #endif // ADVANCED_SD_COMMAND

  public: /** Public Function */

    void init(int8_t pin, uint8_t spi_divisor, int8_t detect_pin);

    void mount();
    void unmount();
    void ls();

    void manage_sd();

    void getfilename(uint16_t nr, PGM_P const match=nullptr);
    void getAbsFilename(char * name);
    void startFilePrint();
    void endFilePrint();
    void write_command(char * buf);
    void print_status();
    bool startWrite(const char * const path, const bool silent=false);
    void deleteFile(const char * const path);
    void deleteFileOrDir(const char * const path);
    void finishWrite();
    void makeDirectory(const char * const path);
    void deleteAllFilesInWorkDirectory();
    bool exists(const char * const path);
    void closeFile();
    void fileHasFinished();
    void chdir(const char * const relpath);
    void reset_default();
    void beginautostart();
    void checkautostart();
    void setroot();
    void setlast();

    static void printEscapeChars(PGM_P s);

    bool selectFile(const char * const path, const bool silent=false);

    int8_t updir();
    uint16_t getnrfilenames();
    uint16_t get_num_Files();

    #if HAS_SD_RESTART
      void open_restart_file(const bool read);
      void delete_restart_file();
      bool exist_restart_file();
    #endif

    #if HAS_EEPROM_SD
      void import_eeprom();
      void write_eeprom();
    #endif

    #if ENABLED(SDCARD_SORT_ALPHA)
      void presort();
      void getfilename_sorted(const uint16_t nr);
      #if ENABLED(SDSORT_GCODE)
      	inline bool getSortOn() { return sort_alpha; }
        inline void setSortOn(const bool b, const bool presortNow = true) { sort_alpha = b; if (presortNow) presort(); }
        inline void setSortFolders(const int i) { sort_folders = i; presort(); }
        //FORCE_INLINE void setSortReverse(const bool b) { sort_reverse = b; }
      #endif
    #else
      inline void getfilename_sorted(const uint16_t nr) { getfilename(nr); }
    #endif

    // Card flag bit 0 SD Mounted
    FORCE_INLINE void setMounted(const bool onoff) { flags.Mounted = onoff; }
    FORCE_INLINE bool isMounted() { return flags.Mounted; }

    // Card flag bit 1 printing
    FORCE_INLINE void setPrinting(const bool onoff) { flags.Printing = onoff; }
    FORCE_INLINE bool isPrinting() { return flags.Printing; }

    // Card flag bit 2 print complete
    FORCE_INLINE void setComplete(const bool onoff) { flags.PrintComplete = onoff; }
    FORCE_INLINE bool isComplete() { return flags.PrintComplete; }

    // Card flag bit 3 Autoreport
    FORCE_INLINE void setAutoreport(const bool onoff) { flags.Autoreport = onoff; }
    FORCE_INLINE bool isAutoreport() { return flags.Autoreport; }

    // Card flag bit 4 Filename is dir
    FORCE_INLINE void setFilenameIsDir(const bool onoff) { flags.FilenameIsDir = onoff; }
    FORCE_INLINE bool isFilenameIsDir() { return flags.FilenameIsDir; }

    inline void pauseSDPrint() { setPrinting(false); }
    inline bool isFileOpen()   { return isMounted() && gcode_file.isOpen(); }
    inline bool isPaused()     { return isFileOpen() && !isPrinting(); }
    inline void setIndex(uint32_t newpos) { sdpos = newpos; gcode_file.seekSet(sdpos); }
    inline uint32_t getIndex() { return sdpos; }
    inline bool eof() { return sdpos >= fileSize; }
    inline int16_t get() { sdpos = gcode_file.curPosition(); return (int16_t)gcode_file.read(); }
    inline uint8_t percentDone() { return (isFileOpen() && fileSize) ? sdpos / ((fileSize + 99) / 100) : 0; }
    inline void getWorkDirName() { workDir.getName(fileName, LONG_FILENAME_LENGTH); }
    inline size_t read(void* buf, uint16_t nbyte) { return gcode_file.isOpen() ? gcode_file.read(buf, nbyte) : -1; }
    inline size_t write(void* buf, uint16_t nbyte) { return gcode_file.isOpen() ? gcode_file.write(buf, nbyte) : -1; }

    inline float fractionDone() {
        float fractionprinted;
        if (fileSize < 2000000) {
          fractionprinted = (float)sdpos / (float)fileSize;
        }
        else fractionprinted = (float)(sdpos >> 8) / (float)(fileSize >> 8);
        return fractionprinted;
    }

    inline bool isInserted() {
      #if PIN_EXISTS(SD_DETECT)
        #if ENABLED(SD_DETECT_INVERTED)
          return detectPin!=-1 ? READ(detectPin) : true;
        #else
          return detectPin!=-1 ? !READ(detectPin) : true;
        #endif
      #else
        //No card detect line? Assume the card is inserted.
        return true;
      #endif
    }

    #if ENABLED(ADVANCED_SD_COMMAND)
      // Format SD Card
      void formatSD();
      // Info SD Card
      void infoSD();
    #endif

  private: /** Private Function */

    void openFailed(const char * const path);
    void lsRecursive(FatFile *dir, uint8_t level=0);
    void lsDive(SdFile parent, PGM_P const match = NULL);
    void parsejson(SdFile &parser_file);
    bool findGeneratedBy(char* buf, char* genBy);
    bool findFirstLayerHeight(char* buf, float &firstlayerHeight);
    bool findLayerHeight(char* buf, float &layerHeight);
    bool findFilamentNeed(char* buf, float &filament);
    bool findTotalHeight(char* buf, float &objectHeight);
    
    void readFileInfo(SdFile &file);
    void clearFileInfo();
    bool findFileInfo(const char *buf, uint16_t buf_length);

    static int16_t getFileModifyDate(SdFile& file);
    static int16_t getFileModifyTime(SdFile& file);

    #if ENABLED(SDCARD_SORT_ALPHA)
      void flush_presort();
    #endif

    #if ENABLED(ADVANCED_SD_COMMAND)

      // write cached block to the card
      uint8_t writeCache(uint32_t lbn) {
        return sd.writeBlock(lbn, cache.data);
      }
      // return cylinder number for a logical block number
      uint16_t lbnToCylinder(uint32_t lbn) {
        return lbn / (numberOfHeads * sectorsPerTrack);
      }
      // return head number for a logical block number
      uint8_t lbnToHead(uint32_t lbn) {
        return (lbn % (numberOfHeads * sectorsPerTrack)) / sectorsPerTrack;
      }
      // return sector number for a logical block number
      uint8_t lbnToSector(uint32_t lbn) {
        return (lbn % sectorsPerTrack) + 1;
      }
      // generate serial number from card size and micros since boot
      uint32_t volSerialNumber() {
        return (cardSizeBlocks << 8) + micros();
      }

      // Info SD Card
      uint8_t cidDmp();
      uint8_t csdDmp();
      void volDmp();

      // initialize appropriate sizes for SD capacity
      void initSizes();

      // zero cache and optionally set the sector signature
      void clearCache(uint8_t addSig);

      // zero FAT and root dir area on SD
      void clearFatDir(uint32_t bgn, uint32_t count);

      // format and write the Master Boot Record
      void writeMbr();

      // format the SD as FAT16
      void makeFat16();
      // format the SD as FAT32
      void makeFat32();

    #endif

};



#else

#define IS_SD_PRINTING()  false
#define IS_SD_PAUSED()    false
#define IS_SD_FILE_OPEN() false
#define IS_SD_MOUNTED()   false

#endif // HAS_SD_SUPPORT
