#ifndef __OCADFILESTRUCTS_H__
#define __OCADFILESTRUCTS_H__

#ifndef USING_PCH
#include <stdint.h>
#endif /* USING_PCH */

#include "PascalString.H"

/*!\brief   A special data type used for all coordinates.

              It is defined as

              TCord = record x, y: longint; end;

              The lowest 8 Bits are used to mark special points:
              Marks for the x-coordinate:
              - 1: this point is the first curve point<br>
              - 2: this point is the second curve point<br>
              - 4: for double lines: there is no left line
                   between this point and the next point
	      .
              Marks for y-coordinate:
              - 1: this point is a corner point
              - 2: this point is the first point of a hole in
                   an area
              - 4: for double lines: there is no right line
                   between this point and the next point
              - 8: OCAD 7 only: this point is a dash point
	      .
              The upper 24 bits contain the coordinate value
              measured in units of 0.01 mm

Note: Some convenience methods are added to this struct.
	      */
struct TCord 
{
  int32_t x, y;
  //! X coordinate (upper 24bits).
  int xCoord() const { return x >> 8; }
  //! Set X coordinate (upper 24bits).
  void setXCoord(int xc) { x = (x & 0xFF) | (xc << 8); }
  //! Y coordinate (upper 24bits).
  int yCoord() const { return y >> 8; }
  //! Set Y coordinate (upper 24bits).
  void setYCoord(int yc) { y = (y & 0xFF) | (yc << 8); }
  //! Get flag value. \see TCord
  bool isFirstCurvePoint() const { return !!(x & 1U); }
  //! Set flag value. \see TCord
  void setFirstCurvePoint(bool v) { x = (x & ~1U) | !!v; }
  //! Get flag value. \see TCord
  bool isSecondCurvePoint() const { return !!(x & 2U); }
  //! Set flag value. \see TCord
  void setSecondCurvePoint(bool v) { x = (x & ~2U) | (!!v << 1); }
  //! Get flag value. \see TCord
  bool isDoubleLineHasNoLeft() const { return !!(x & 4U); }
  //! Set flag value. \see TCord
  void setDoubleLineHasNoLeft(bool v) { x = (x & ~4U) | (!!v << 2); }
  //! Get flag value. \see TCord
  bool isCornerPoint() const { return !!(y & 1U); }
  //! Set flag value. \see TCord
  void setCornerPoint(bool v) { y = (y & ~1U) | !!v; }
  //! Get flag value. \see TCord
  bool isFirstHolePoint() const { return !!(y & 2U); }
  //! Set flag value. \see TCord
  void setFirstHolePoint(bool v) { y = (y & ~2U) | (!!v << 1); }
  //! Get flag value. \see TCord
  bool isDoubleLineHasNoRight() const { return !!(y & 4U); }
  //! Set flag value. \see TCord
  void setDoubleLineHasNoRight(bool v) { y = (y & ~4U) | (!!v << 2); }
  //! Get flag value. \see TCord
  bool isDashPoint() const { return !!(y & 8U); }
  //! Set flag value. \see TCord
  void setDashPoint(bool v) { y = (y & ~8U) | (!!v << 3); }
};

//! \see TBaseSym::Cols
struct TColors
{
  // Pascal set data.
  uint8_t data[32];
  // Test whether the integer value is in the set.
  bool in(int index)
  {
    return data[index/8] & (1<<(index%8));
  }
  // Add the integer value into the set.
  void set(int index, bool v)
  {
    data[index/8] &= (~1u<<(index%8));
    data[index/8] |= ((!!v)<<(index%8));
  }
};

//!  Colors are defined as CMYK values.
struct TCmyk {
  uint8_t cyan;            /*!< 2 times the cyan value as it
			      appears in the Define Color dialog
			      box (to allow half percents) */
  uint8_t magenta;         /*!< dito for magenta, \see cyan */
  uint8_t yellow;          /*!< dito for yellow, \see cyan */
  uint8_t black;           /*!< dito for black, \see cyan */
};

//! The TColorInfo structure is defined as follows.
struct TColorInfo {
  int16_t ColorNum;        /*!< Color number. This number is
			      used in the symbols when
			      referring a color. */
  int16_t Reserved;
  TCmyk Color;              /*!< Color value. The structure
			       is explained below. */
  pascal_string<31> ColorName;     /*!< Description of the color */
  uint8_t SepPercentage[32];
  /*!< Definition how the color
     appears in the different spot
     color separations.
     0..200: 2 times the separation
     percentage as it appears
     in the Color dialog box (to
     allow half percents)
     .255: the color does not
     appear in the corresponding
     color separation (empty field
     in the color dialog box) */
};

struct TColorSep {
  pascal_string<15> SepName;       /*!< Name of the color separation */
  TCmyk Color;              /*!< 0 in OCAD 6, CMYK value of
			       the separation in OCAD 7.
			       This value is only used in
			       the AI (Adobe Illustrator) export */
  int16_t RasterFreq;      /*!< 10 times the halfton frequency
			      as it appears in the Color
			      Separation dialog box. */
  int16_t RasterAngle;     /*!< 10 times the halftone angle
			      as it appears in the Color
			      Separation dialog box. */
};

struct TFileHeader {
  int16_t OCADMark;        /*!< 3245 (hex 0cad) */
  int16_t SectionMark;     /*!< OCAD 6: 0
			      OCAD 7: 7
			      OCAD 8: 2 for normal files
			      3 for course setting files */
  int16_t Version;         /*!< 6 for OCAD 6, 7 for OCAD 7, 8 for OCAD 8 */
  int16_t Subversion;      /*!< number of subversion (0 for 6.00,
			      1 for 6.01 etc.) */
  int32_t FirstSymBlk;      /*!< file position of the first symbol
			       block */
  int32_t FirstIdxBlk;      /*!< file position of the first index
			       block */
  int32_t SetupPos;         /*!< file position of the setup record  */
  int32_t SetupSize;        /*!< size (in bytes) of the setup record */
  int32_t InfoPos;          /*!< file position of the file
			       information. The file information is
			       stored as a zero-terminated string with
			       up to 32767 characters + terminating
			       zero */
  int32_t InfoSize;         /*!< size (in bytes) of the file
			       information */
  int32_t FirstStIndexBlk;  /*!< OCAD 8 only. file position of the first
			       string index block */
  int32_t Reserved2;
  int32_t Reserved3;
  int32_t Reserved4;
};

struct TSymHeader {
  int16_t nColors;         /* Number of colors defined */
  int16_t nColorSep;       /* Number or color separations
			      defined */
  int16_t CyanFreq;        /* Halftone frequency of the
			      Cyan color separation. This
			      is 10 times the value entered
			      in the CMYK Separations dialog
			      box. */
  int16_t CyanAng;         /* Halftone angle of the cyan
			      color separation. This is 10 times
			      the value entered in the CMYK
			      separations dialog box. */
  int16_t MagentaFreq;     /* dito for magenta */
  int16_t MagentaAng;      /* dito for magenta */
  int16_t YellowFreq;      /* dito for yellow */
  int16_t YellowAng;       /* dito for yellow */
  int16_t BlackFreq;       /* dito for black */
  int16_t BlackAng;        /* dito for black */
  int16_t Res1;
  int16_t Res2;
  TColorInfo aColorInfo[256];
  /* the TColorInfo structure is
     explained below */
  TColorSep aColorSep[32];
  /* the TColorSep structure is
     explained below. Note that only
     24 color separations are allowed.
     The rest is reserved for future
     use. */
};

struct TSymbolBlock {
  int32_t NextBlock;        /* file position of the next symbol
			       block. 0 if this is the last
			       symbol block. */
  int32_t FilePos[256];
  /* file position of up to 256
     symbols. 0 if there is no symbol
     for this index. */
};

struct TBaseSym {
  int16_t Size;            /* Size of the symbol in bytes. This
			      depends on the type and the
			      number of subsymbols. */
  int16_t Sym;             /* Symbol number. This is 10 times
			      the value which appears on the
			      screen (1010 for 101.0) */
  int16_t Otp;             /* Object type
			      .1: Point symbol
			      .2: Line symbol or Line text symbol
			      .3: Area symbol
			      .4: Text symbol
			      .5: Rectangle symbol */
  uint8_t SymTp;           /* Symbol type
			      .1: for Line text and text
			      symbols
			      .0: for all other symbols */
  uint8_t Flags;           /* OCAD 6/7: must be 0
			      OCAD 8: bit flags
			      .1: not oriented to north (inverted for
			      better compatibility)
			      .2: Icon is compressed */
  int16_t Extent;          /* Extent how much the rendered
			      symbols can reach outside the
			      coordinates of an object with
			      this symbol.
			      For a point object it tells
			      how far away from the coordinates
			      of the object anything of the
			      point symbol can appear. */
  bool Selected;            /* Symbol is selected in the symbol
			       box */
  uint8_t Status;          /* Status of the symbol
			      .0: Normal
			      .1: Protected
			      .2: Hidden */
  int16_t Res2;
  int16_t Res3;
  int32_t FilePos;          /*!< File position, not used in the
			       file, only when loaded in
			       memory. Value in the file is
			       not defined. */
  TColors Cols;             /*!< Set of the colors used in this
			       symbol. TColors is an array of
			       32 bytes, where each bit
			       represents 1 of the 256 colors.
			       TColors = set of 0..255;
			       The color with the number 0 in
			       the color table appears as the
			       lowest bit in the first byte of
			       the structure. */
  pascal_string<31> Description;  /* The description of the symbol */
  uint8_t IconBits[264];   /* the icon can be uncompressed (16-bit colors)
			      or compressed (256 color palette) depending
			      on the Flags field.
			      In OCAD 6/7 it is always uncompressed */
};

struct TPointSym {
  int16_t Size;            /* size of this structure including
			      the objects following this
			      structure */
  int16_t Sym;             /* see TBaseSym */
  int16_t Otp;             /* see TBaseSym */
  uint8_t SymTp;           /* see TBaseSym */
  uint8_t Flags;           /* see TBaseSym */
  int16_t Extent;          /* see TBaseSym */
  bool Selected;            /* Symbol is selected in the symbol
			       box */
  uint8_t Status;          /* Status of the symbol
			      .0: Normal
			      .1: Protected
			      .2: Hidden */
  int16_t Res2;
  int16_t Res3;
  int32_t FilePos;          /* see TBaseSym */
  TColors Cols;             /* see TBaseSym */
  pascal_string<31> Description;  /* see TBaseSym */
  uint8_t IconBits[264];
  /* see TBaseSym */
  int16_t DataSize;        /* number of coordinates (each 8 bytes)
			      which follow this structure,
			      each object header counts as
			      2 Coordinates (16 bytes).
			      The maximum value is 512 */
  int16_t Reserved;
};

struct TSymElt {
  int16_t stType;          /* type of the object
			      .1: line
			      .2: area
			      .3: circle
			      .4: dot (filled circle) */
  uint16_t stFlags;         /* Flags
			       .1: line with round ends */
  int16_t stColor;         /* color of the object. This is the
			      number which appears in the Colors
			      dialog box */
  int16_t stLineWidth;     /* line width for lines and circles
			      unit 0.01 mm */
  int16_t stDiameter;      /* diameter for circles and dots. The
			      line width is included in this
			      dimension for circles. Unit 0.01 mm */
  int16_t stnPoly;         /* number of coordinates */
  int16_t stRes1;
  int16_t stRes2;
  TCord stPoly[512];        /* coordinates of the object */
};

struct TLineSym {
  int16_t Size;            /* size of this structure including
			      the objects following this
			      structure */
  int16_t Sym;             /* see TBaseSym */
  int16_t Otp;             /* 2 */
  uint8_t SymTp;           /* 0 */
  uint8_t Flags;           /* see TBaseSym */
  int16_t Extent;          /* see TBaseSym */
  bool Selected;            /* Symbol is selected in the symbol
			       box */
  uint8_t Status;          /* Status of the symbol
			      .0: Normal
			      .1: Protected
			      .2: Hidden */
  int16_t Tool;            /* OCAD 6: reserved
			      OCAD 7/8: Preferred drawing tool
			      .0: off
			      .1: Curve mode
			      .2: Ellipse mode
			      .3: Circle mode
			      .4: Rectangular mode
			      .5: Straight line mode
			      .6: Freehand mode */
  int16_t Res3;
  int32_t FilePos;          /* see TBaseSym */
  TColors Cols;             /* see TBaseSym */
  pascal_string<31> Description;  /* see TBaseSym */
  uint8_t IconBits[264];
  /* see TBaseSym */
  int16_t LineColor;       /* Line color */
  int16_t LineWidth;       /* Line width */
  uint16_t LineEnds;        /* true if Round line ends is checked */
  int16_t DistFromStart;   /* Distance from start */
  int16_t DistToEnd;       /* Distance to the end */
  int16_t MainLength;      /* Main length a */
  int16_t EndLength;       /* End length b */
  int16_t MainGap;         /* Main gap C */
  int16_t SecGap;          /* Gap D */
  int16_t EndGap;          /* Gap E */
  int16_t MinSym;          /* -1: at least 0 gaps/symbols
			      .0: at least 1 gap/symbol
			      .1: at least 2 gaps/symbols
			      etc.
			      for OCAD 6 only the values 0 and 1 are
			      allowed */
  int16_t nPrimSym;        /* No. of symbols */
  int16_t PrimSymDist;     /* Distance */
  uint16_t DblMode;         /* Mode (Double line page) */
  uint16_t DblFlags;        /* low order bit is set if
			       Fill is checked */
  int16_t DblFillColor;    /* Fill color */
  int16_t DblLeftColor;    /* Left line/Color */
  int16_t DblRightColor;   /* Right line/Color */
  int16_t DblWidth;        /* Width */
  int16_t DblLeftWidth;    /* Left line/Line width */
  int16_t DblRightWidth;   /* Right line/Line width */
  int16_t DblLength;       /* Dashed/Distance a */
  int16_t DblGap;          /* Dashed/Gap */
  int16_t DblRes[3];
  uint16_t DecMode;         /* Decrease mode
			       .0: off
			       .1: decreasing towards the end
			       .2: decreasing towards both ends */
  int16_t DecLast;         /* Last symbol */
  int16_t DecRes;          /* Reserved */
  int16_t FrColor;         /* OCAD 6: reserved
			      OCAD 7/8: color of the framing line */
  int16_t FrWidth;         /* OCAD 6: reserved
			      OCAD 7/8: Line width of the framing line */
  int16_t FrStyle;         /* OCAD 6: reserved
			      OCAD 7/8: Line style of the framing line
			      .0: flat cap/bevel join
			      .1: round cap/round join
			      .4: flat cap/miter join */
  int16_t PrimDSize;       /* number or coordinates (8 bytes)
			      for the Main symbol A which
			      follow this structure
			      Each symbol header counts as
			      2 coordinates (16 bytes).
			      The maximum value is 512. */
  int16_t SecDSize;        /* number or coordinates (8 bytes)
			      for the Secondary symbol which
			      follow the Main symbol A
			      Each symbol header counts as
			      2 coordinates (16 bytes).
			      The maximum value is 512. */
  int16_t CornerDSize;     /* number or coordinates (8 bytes)
			      for the Corner symbol which
			      follow the Secondary symbol
			      Each symbol header counts as
			      2 coordinates (16 bytes).
			      The maximum value is 512. */
  int16_t StartDSize;      /* number or coordinates (8 bytes)
			      for the Start symbol C which
			      follow the Corner symbol
			      Each symbol header counts as
			      2 coordinates (16 bytes).
			      The maximum value is 512. */
  int16_t EndDSize;        /* number or coordinates (8 bytes)
			      for the End symbol D which
			      follow the Start symbol C
			      Each symbol header counts as
			      2 coordinates (16 bytes).
			      The maximum value is 512. */
  int16_t Reserved;
};

struct TLTextSym {
  int16_t Size;            /* size of this structure */
  int16_t Sym;             /* see TBaseSym */
  int16_t Otp;             /* 2 */
  uint8_t SymTp;           /* 1 */
  uint8_t Flags;           /* see TBaseSym */
  int16_t Extent;          /* see TBaseSym */
  bool Selected;            /* Symbol is selected in the symbol
			       box */
  uint8_t Status;          /* Status of the symbol
			      .0: Normal
			      .1: Protected
			      .2: Hidden */
  int16_t Tool;            /* OCAD 6: reserved
			      OCAD 7/8: Preferred drawing tool
			      .0: off
			      .1: Curve mode
			      .2: Ellipse mode
			      .3: Circle mode
			      .4: Rectangular mode
			      .5: Straight line mode
			      .6: Freehand mode */
  int16_t FrWidth;         /* OCAD 6: reserved
			      OCAD 7: Framing width
			      sorry, this had to be squeezed
			      in here
			      OCAD 8: not used */
  int32_t FilePos;          /* see TBaseSym */
  TColors Cols;             /* see TBaseSym */
  pascal_string<31> Description;  /* see TBaseSym */
  uint8_t IconBits[264];
  /* see TBaseSym */
  pascal_string<31> FontName;      /* TrueType font */
  int16_t FontColor;       /* Color */
  int16_t FontSize;        /* 10 times the value entered in Size */
  int16_t Weight;          /* Bold as used in the Windows GDI
			      .400: normal
			      .700: bold */
  bool Italic;              /* true if Italic is checked */
  uint8_t CharSet;         /* OCAD 6/7: must be 0
			      OCAD 8: CharSet of the text, if the text is
			      not Unicode */
  int16_t CharSpace;       /* Char. spacing */
  int16_t WordSpace;       /* Word spacing */
  int16_t Alignment;       /* Alignment
			      .0: Left
			      .1: Center
			      .2: Right
			      .3: All line */
  int16_t FrMode;          /* Framing mode
			      .0: no framing
			      .1: framing with a framing font
			      .2: OCAD 7/8 only: framing with a line 
			      Note this feature is called
			      "Second font" in OCAD 6 but
			      "Framing" in OCAD 7 */
  pascal_string<31> FrName;        /* OCAD 6/7: TrueType font (Second/Framing font)
				      OCAD 8: not used */
  int16_t FrColor;         /* Color (Second/Framing font) */
  int16_t FrSize;          /* OCAD 6/7: Size (Second/Framing font)
			      OCAD 8: Framing width */
  int16_t FrWeight;        /* OCAD 6/7: Bold (Second/Framing font)
			      as used in the Windows GDI
			      .400: normal
			      .700: bold
			      OCAD 8: not used */
  uint16_t FrItalic;        /* OCAD 6/7: true if Italic is checked
			       (Second/Framing font)
			       OCAD 8: not used */
  int16_t FrOfX;           /* OCAD 6/7: Horizontal offset
			      OCAD 8: not used */
  int16_t FrOfY;           /* OCAD 6/7: Vertical offset
			      OCAD 8: not used */
};

struct TAreaSym {
  int16_t Size;            /* size of this structure including
			      the objects following this
			      structure */
  int16_t Sym;             /* see TBaseSym */
  int16_t Otp;             /* 3 */
  uint8_t SymTp;           /* 0 */
  uint8_t Flags;           /* see TBaseSym */
  int16_t Extent;          /* see TBaseSym */
  bool Selected;            /* Symbol is selected in the symbol
			       box */
  uint8_t Status;          /* Status of the symbol
			      .0: Normal
			      .1: Protected
			      .2: Hidden */
  int16_t Tool;            /* OCAD 6: reserved
			      OCAD 7/8: Preferred drawing tool
			      .0: off
			      .1: Curve mode
			      .2: Ellipse mode
			      .3: Circle mode
			      .4: Rectangular mode
			      .5: Straight line mode
			      .6: Freehand mode */
  int16_t Res3;
  int32_t FilePos;          /* see TBaseSym */
  TColors Cols;             /* see TBaseSym */
  pascal_string<31> Description;   /* see TBaseSym */
  uint8_t IconBits[264];
  /* see TBaseSym */
  uint16_t AreaFlags;       /* reserved */
  uint16_t FillOn;          /* true if Fill background is
			       checked */
  int16_t FillColor;       /* Fill color */
  int16_t HatchMode;       /* Hatch mode
			      .0: None
			      .1: Single hatch
			      .2: Cross hatch */
  int16_t HatchColor;      /* Color (Hatch page) */
  int16_t HatchLineWidth;  /* Line width */
  int16_t HatchDist;       /* Distance */
  int16_t HatchAngle1;     /* Angle 1 */
  int16_t HatchAngle2;     /* Angle 2 */
  int16_t HatchRes;
  int16_t StructMode;      /* Structure
			      .0: None
			      .1: aligned rows
			      .2: shifted rows */
  int16_t StructWidth;     /* Width */
  int16_t StructHeight;    /* Height */
  int16_t StructAngle;     /* Angle */
  int16_t StructRes;
  int16_t DataSize;        /* number of coordinates (each 8 bytes)
			      which follow this structure,
			      each object header counts as
			      2 Coordinates (16 bytes).
			      The maximum value is 512. */
};

struct TTextSym {
  int16_t Size;            /* size of this structure */
  int16_t Sym;             /* see TBaseSym */
  int16_t Otp;             /* 4 */
  uint8_t SymTp;           /* 1 */
  uint8_t Flags;           /* see TBaseSym */
  int16_t Extent;          /* 0 */
  bool Selected;            /* Symbol is selected in the symbol
			       box */
  uint8_t Status;          /* Status of the symbol
			      .0: Normal
			      .1: Protected
			      .2: Hidden */
  int16_t Res2;
  int16_t FrWidth;         /* OCAD 6: reserved
			      OCAD 7: Framing width
			      sorry, this had to be squeezed
			      in here
			      OCAD 8: not used */
  int32_t FilePos;          /* see TBaseSym */
  TColors Cols;             /* see TBaseSym */
  pascal_string<31> Description;  /* see TBaseSym */
  uint8_t IconBits[264];
  /* see TBaseSym */
  pascal_string<31> FontName;      /* TrueType font */
  int16_t FontColor;       /* Color */
  int16_t FontSize;        /* 10 times the size in pt */
  int16_t Weight;          /* Bold as used in the Windows GDI
			      .400: normal
			      .700: bold */
  bool Italic;              /* true if Italic is checked */
  uint8_t CharSet;         /* OCAD 6/7: must be 0
			      OCAD 8: CharSet of the text, if the text is
			      not Unicode */
  int16_t CharSpace;       /* Char. spacing */
  int16_t WordSpace;       /* Word spacing */
  int16_t Alignment;       /* Alignment
			      .0: Left
			      .1: Center
			      .2: Right
			      .3: Justified */
  int16_t LineSpace;       /* Line spacing */
  int16_t ParaSpace;       /* Space after Paragraph */
  int16_t IndentFirst;     /* Indent first line */
  int16_t IndentOther;     /* Indent other lines */
  int16_t nTabs;           /* number of Tabulators */
  int32_t Tabs[32];
  /* Tabulators */
  uint16_t LBOn;            /* true if Line below On is checked */
  int16_t LBColor;         /* Line color (Line below) */
  int16_t LBWidth;         /* Line width (Line below) */
  int16_t LBDist;          /* Distance from text */
  int16_t Res4;
  int16_t FrMode;          /* Framing mode
			      .0: no framing
			      .1: framing with a framing font
			      .2: OCAD 7/8 only: framing with a line
			      Note this feature is called
			      "Second font" in OCAD 6 but
			      "Framing" in OCAD 7/8 */
  pascal_string<31> FrName;        /* OCAD 6/7: TrueType font (Second/Framing font)
				      OCAD 8: not used */
  int16_t FrColor;         /* Color (Second/Framing font) */
  int16_t FrSize;          /* OCAD 6/7: Size (Second/Framing font)
			      OCAD 8: framing width */
  int16_t FrWeight;        /* OCAD 6/7: Bold (Second/Framing font)
			      .400: normal
			      .700: bold
			      OCAD 8: not used */
  uint16_t FrItalic;        /* true if Second/Framing font Italic
			       is checked */
  int16_t FrOfX;           /* OCAD 6/7: Horizontal offset
			      OCAD 8: not used */
  int16_t FrOfY;           /* OCAD 6/7: Vertical offset
			      OCAD 8: not used */
};

struct TRectSym {
  int16_t Size;            /* size of this structure */
  int16_t Sym;             /* see TBaseSym */
  int16_t Otp;             /* 5 */
  uint8_t SymTp;           /* 0 */
  uint8_t Flags;           /* see TBaseSym */
  int16_t Extent;          /* see TBaseSym */
  bool Selected;            /* Symbol is selected in the symbol
			       box */
  uint8_t Status;          /* Status of the symbol
			      .0: Normal
			      .1: Protected
			      .2: Hidden */
  int16_t Res2;
  int16_t Res3;
  int32_t FilePos;          /* see TBaseSym */
  TColors Cols;             /* see TBaseSym */
  pascal_string<31> Description;  /* see TBaseSym */
  uint8_t IconBits[264];   /* see TBaseSym */
  int16_t LineColor;       /* Line color */
  int16_t LineWidth;       /* Line width */
  int16_t Radius;          /* Corner radius */
  uint16_t GridFlags;       /* Flags
			       .1: Grid On
			       .2: Numbered from the bottom */
  int16_t CellWidth;       /* Cell width */
  int16_t CellHeight;      /* Cell height */
  int16_t ResGridLineColor;
  int16_t ResGridLineWidth;
  int16_t UnnumCells;      /* Unnumbered Cells */
  pascal_string<3> UnnumText; /* Text in unnumbered Cells */
  int16_t GridRes2;
  pascal_string<31> ResFontName;
  int16_t ResFontColor;
  int16_t ResFontSize;
  int16_t ResWeight;
  uint16_t ResItalic;
  int16_t ResOfsX;
  int16_t ResOfsY;
};

struct TIndex {
  TCord LowerLeft;          /* lower left corner of
			       a rectangle covering the
			       entire object. All flag bits
			       are set to 0 */
  TCord UpperRight;         /* upper right corner of
			       a rectangle covering the
			       entire object. All flag bits
			       are set to 0 */
  int32_t Pos;              /* file position of the object */
  uint16_t Len;             /* OCAD 6 and 7: size of the object in the file
			       in bytes
			       OCAD 8: number of coordinate pairs, the size of
			       the object in the file is then calculated by:
			       32 + 8*Len
Note: this is reserved space in the file, the
effective length of the object may be shorter */
  int16_t Sym;             /* 10 times the symbol number
			      Deleted objects are marked with Sym=0 */
};

struct TIndexBlock {
  int32_t NextBlock;        /* file position of the next block
			       0 if this is the last block */
  TIndex IndexArr[256];
  /* TIndex as defined below */
};

struct TElement {
  int16_t Sym;             /* 10 times the symbol number */
  uint8_t Otp;             /* object type
			      .1: point object
			      .2: line or line text object
			      .3: area object
			      .4: unformatted text object
			      .5: formatted text object
			      or rectangle object */
  uint8_t Unicode;         /* OCAD 6/7: must be 0
			      OCAD 8: 1 if the text is Unicode */
  int16_t nItem;           /* number of coordinates in the
			      Poly array */
  int16_t nText;           /* number of coordinates in the
			      Poly array used for storing text
			      nText is > 0 for
			      - line text objects
			      - unformatted text objects
			      - formatted text objects
			      for all other objects it is 0 */
  int16_t Ang;             /* Angle, unit is 0.1 degrees
			      used for
			      - point object
			      - area objects with structure
			      - unformatted and formatted
			      text objects
			      - rectangle objects */
  int16_t Res1;
  int32_t ResHeight;        /* reserved for future use to
			       store a height information */
  pascal_string<15> ResId;         /* reserved */
  TCord Poly[2000];         /* OCAD 8: array[0..32767]    
			       coordinates of the object
			       followed by a zero-terminated
			       string if nText > 0
			       TCord is explained at the beginning
			       of this description */
};

struct TGpsAdjPoint {
  TCord lpMap;              /* Point on the map. All flag bits
			       are set to 0 */
  double Lat;               /* Latitude reported by the GPS
			       receiver in this point (degrees) */
  double Long;              /* Longitude reported by the GPS
			       receiver in this point (degrees) */
  pascal_string<15> Name;          /* name of the adjustment point as
				      it appears in the Adjust GPS dialog
				      box */
};

struct TZoomRec {
  double Zoom;              /* Zoom magnification as it appears
			       in the View menu and in the
			       status bar */
  TCord Offset;             /* Coordinate of the center of
			       the screen. All flag bits are
			       set to 0 */
};

struct TStp {
  TCord Offset;             /* Coordinate of the center
			       of the screen when the file was
			       last saved. All flag bits
			       are set to 0 */
  double rGridDist;         /* Grid distance for paper
			       paper coordinates in mm */
  int16_t WorkMode;        /* Mode when last the file was
			      last saved
			      .5: freehand
			      .6: straight
			      .7: rectangular
			      .8: circle
			      .9: ellipse
			      .10: curve
			      .11: edit point
			      .12: edit object */
  int16_t LineMode;        /* drawing mode when the file was
			      last saved
			      .5: freehand
			      .6: straight
			      .7: rectangular
			      .8: circle
			      .9: ellipse
			      .10: curve */
  int16_t EditMode;        /* edit mode when the file was
			      last saved
			      .11: edit point
			      .12: edit object */
  int16_t ActSym;          /* selected symbol the file was
			      last saved */
  double MapScale;          /* Map scale */
  double RealWorldOfsX;     /* Horizontal offset of real
			       world coordinates in meters */
  double RealWorldOfsY;     /* Vertical offset of real
			       world coordinates in meters */
  double RealWorldAngle;    /* Angle of real world coordinates
			       in degrees */
  double RealWorldGrid;     /* Grid distance of real world
			       coordinates in meters */
  double GpsAngle;          /* Angle of the GPS adjustment as
			       displayed in the Adjust GPS
			       dialog box */
  TGpsAdjPoint aGpsAdjust[12];
  /* GPS adjustment points, this
     structure is defined below */
  int32_t nGpsAdjust;       /* number of GPS adjustment points */
  double DraftScaleX;       /* Horizontal draft scale, not used in OCAD 8 */
  double DraftScaleY;       /* Vertical draft scale, not uses in OCAD 8 */
  TCord TempOffset;         /* Template offset. This defines
			       the coordinates where the center
			       of the template is displayed, not used in OCAD 8 */
  pascal_string<255> TemplateFileName;
  /* file name of the template, not used in OCAD 8 */
  uint16_t TemplateEnabled; /* true if a template is opened, not used in OCAD 8 */
  int16_t TempResol;       /* resolution of the template file
			      in DPI, not used in OCAD 8 */
  double rTempAng;          /* Angle of the adjusted template, 
			       not used in OCAD 8 */
  TCord Reserved0;
  double Reserved1;
  TCord PrLowerLeft;        /* lower left corner of the print
			       window. All Flag bits are set
			       to 0 */
  TCord PrUpperRight;       /* upper right corner of the print
			       window. All Flag bits are set
			       to 0 */
  uint16_t PrGrid;          /* true if Print grid is activated
			       in the Print dialog box */
  int16_t PrGridColor;     /* Grid color selected in the
			      Print dialog box */
  int16_t PrOverlapX;      /* Horizontal overlap as defined
			      in the Print Window dialog box
			      unit is 0.01 mm */
  int16_t PrOverlapY;      /* Vertical overlap as defined
			      in the Print Window dialog box
			      unit is 0.01 mm */
  double PrintScale;        /* Print scale */
  int16_t PrIntensity;     /* Intensity as defined in the
			      Printing Options dialog box */
  int16_t PrLineWidth;     /* Line Width as defined in the
			      Printing Options dialog box */
  uint16_t PrReserved;
  uint16_t PrStdFonts;      /* OCAD 6: true if Standard fonts as
			       PostScript fonts is activated
			       in the EPS Properties dialog box
			       OCAD7/8: not used */
  uint16_t PrReserved2;
  uint16_t PrReserved3;
  TCord PartialLowerLeft;   /* lower left corner of Export
			       Partial map window. All flag bits
			       are set to 0 */
  TCord PartialUpperRight;  /* upper right corner of the Export
			       Partial map window. All flag bits
			       are set to 0 */
  double Zoom;              /* Zoom magnification as it appears
			       in the View menu and in the
			       status bar */
  TZoomRec ZoomHist[9];     /* Last 8 Zoom magnification for use
			       in the Zoom out command. TZoomRec
			       is explained below. */
  int32_t nZoomHist;        /* number of magnificiations in
			       ZoomHist */

  /* OCAD 6: the setup ends here the following fields exist in OCAD 7 only */

  uint16_t RealWorldCord;   /* true if real world coordinates
			       are to be displayed */
  // TODO - what is ShortString?
  pascal_string<12> FileName; /* used internally in temporary files.
				 The name of the original file
				 is stored here */
  uint16_t HatchAreas;      /* true if Hatch areas is active */
  uint16_t DimTemp;         /* true if Dim template is active */
  uint16_t HideTemp;        /* true if Hide template is active */
  int16_t TempMode;        /* template mode
			      .0: in the background
			      .1: above a color */
  int16_t TempColor;       /* the color if template mode is 1 */
};

struct TStIndex {
  // TODO - what is Cardinal?
  uint32_t Pos;             /* file position of the string */
  uint32_t Len;             /* length reserved for the string. Note that 
			       the string is normally shorter than this
			       reserved length. The string is null terminated. */
  int16_t StType;           /* string type. See the list below */
  int16_t ObjIndex;         /* if the string refers to an object, this
			       is the number of the objects. Note that
			       the numbering of the objects starts with 1.
			       Otherwise this is 0. */
};

struct TStIndexBlk {
  int32_t FilePos;          /* file position of the next StIndexBlk 
			       0 if this is the last StIndexBlk */
  TStIndex Table[256];
};

#endif
