#pragma once

class GMesh ;
class GSubstance ; 
//class GOpticalSurface ;

#include "GNode.hh"
#include "GMatrix.hh"

//  hmm the difference between the models is focussed in here 
//   chroma.geometry.Solid is all about splaying things across all the triangles
//  relationship between how many materials for each mesh is up for grabs
//

class GSolid : public GNode {
  public:
      GSolid( unsigned int index, GMatrixF* transform, GMesh* mesh,  GSubstance* substance);
      virtual ~GSolid();

  public:
     void setSelected(bool selected);
     bool isSelected();

  public:
     void setSubstance(GSubstance* substance);
 //    void setInnerOpticalSurface(GOpticalSurface* ios);
 //    void setOuterOpticalSurface(GOpticalSurface* oos);

  public:
     GSubstance* getSubstance();
 //    GOpticalSurface* getInnerOpticalSurface();
 //    GOpticalSurface* getOuterOpticalSurface();

  public: 
      void Summary(const char* msg="GSolid::Summary");
 
  private:
      GSubstance*        m_substance ; 
 //     GOpticalSurface*   m_inner_optical_surface ; 
 //     GOpticalSurface*   m_outer_optical_surface ; 
      bool m_selected ;

};

inline GSolid::GSolid( unsigned int index, GMatrixF* transform, GMesh* mesh, GSubstance* substance)
         : 
         GNode(index, transform, mesh ),
         m_substance(substance),
//         m_inner_optical_surface(NULL),
//         m_outer_optical_surface(NULL),
         m_selected(true)
{
    // NB not taking ownership yet 
}

inline GSolid::~GSolid()
{
}

inline GSubstance* GSolid::getSubstance()
{
    return m_substance ; 
}



inline void GSolid::setSelected(bool selected)
{
    m_selected = selected ; 
}
inline bool GSolid::isSelected()
{
   return m_selected ; 
}





