// ============================================================================
//
// Copyright (c) 1998 The CGAL Consortium
//
// This software and related documentation is part of an INTERNAL release
// of the Computational Geometry Algorithms Library (CGAL). It is not
// intended for general use.
//
// ----------------------------------------------------------------------------
//
// release       :
// release_date  :
//
// file          : include/CGAL/Triangulation_data_structure_3.h
// revision      : $Revision$
// author(s)     : Monique Teillaud <Monique.Teillaud@sophia.inria.fr>
//
// coordinator   : INRIA Sophia Antipolis (Mariette Yvinec)
//
// ============================================================================
//
// combinatorial triangulation of the boundary of a polytope
// of dimension d in dimension d+1
// for -1 <= d <= 3
//
// ============================================================================

#ifndef CGAL_TRIANGULATION_DATA_STRUCTURE_3_H
#define CGAL_TRIANGULATION_DATA_STRUCTURE_3_H

#include <pair.h>
#include <CGAL/triple.h>
#include <list.h>
#include <map.h>
#include <set.h>
#include <vector.h>

#include <CGAL/Triangulation_utils_3.h>
#include <CGAL/triangulation_assertions.h>
#include <CGAL/Triangulation_short_names_3.h>

#include <CGAL/Triangulation_vertex_base_3.h>
#include <CGAL/Triangulation_cell_base_3.h>

#include <CGAL/Triangulation_ds_cell_3.h>
#include <CGAL/Triangulation_ds_vertex_3.h>

#include <CGAL/Triangulation_ds_iterators_3.h>
#include <CGAL/Triangulation_ds_circulators_3.h>


template <class Tds>
class CGAL_Triangulation_ds_cell_iterator_3;
template <class Tds>
class CGAL_Triangulation_ds_facet_iterator_3;
template <class Tds>
class CGAL_Triangulation_ds_vertex_iterator_3;
template <class Tds>
class CGAL_Triangulation_ds_cell_circulator_3;

template <class Vb, class Cb>
class CGAL_Triangulation_data_structure_3
  :public CGAL_Triangulation_utils_3
{

  friend istream& operator>> CGAL_NULL_TMPL_ARGS
  (istream&, CGAL_Triangulation_data_structure_3<Vb,Cb>&);
//   friend ostream& operator<< CGAL_NULL_TMPL_ARGS
//   (ostream& os, const CGAL_Triangulation_data_structure_3<Vb,Cb> &tr);

  friend void CGAL_Triangulation_ds_cell_3<Vb,Cb>::add_list
  (CGAL_Triangulation_data_structure_3<Vb,Cb>&);

  friend class CGAL_Triangulation_ds_cell_iterator_3
  <CGAL_Triangulation_data_structure_3<Vb,Cb> >;
  friend class CGAL_Triangulation_ds_facet_iterator_3
  <CGAL_Triangulation_data_structure_3<Vb,Cb> >;
  friend class CGAL_Triangulation_ds_edge_iterator_3
  <CGAL_Triangulation_data_structure_3<Vb,Cb> >;
  friend class CGAL_Triangulation_ds_vertex_iterator_3
  <CGAL_Triangulation_data_structure_3<Vb,Cb> >;
  
public:
  
  typedef typename Vb::Point Point;
  
  typedef CGAL_Triangulation_ds_vertex_3<Vb,Cb> Vertex;
  typedef CGAL_Triangulation_ds_cell_3<Vb,Cb> Cell;
  typedef pair<Cell*, int>  Facet;
  typedef CGAL_triple<Cell*, int, int> Edge;

  typedef CGAL_Triangulation_data_structure_3<Vb,Cb> Tds;

  typedef CGAL_Triangulation_ds_cell_iterator_3<Tds> Cell_iterator;
  typedef CGAL_Triangulation_ds_facet_iterator_3<Tds> Facet_iterator;
  typedef CGAL_Triangulation_ds_edge_iterator_3<Tds> Edge_iterator;
  typedef CGAL_Triangulation_ds_vertex_iterator_3<Tds> Vertex_iterator;
  typedef CGAL_Triangulation_ds_cell_circulator_3<Tds> Cell_circulator;

  // CONSTRUCTORS

  inline
  CGAL_Triangulation_data_structure_3() 
    : _dimension(-2), _number_of_vertices(0), _list_of_cells() 
  {}

  CGAL_Triangulation_data_structure_3(const Vertex & v)
    : _dimension(-2), _number_of_vertices(0), _list_of_cells()
  {
    insert_outside_affine_hull(v);
    //    CGAL_triangulation_postcondition( is_valid() );
  }

  inline
  CGAL_Triangulation_data_structure_3(const Tds & tds)
    : _number_of_vertices(0), _list_of_cells()
    // _number_of_vertices is set to 0 so that clear() in copy_tds() works
  {
    copy_tds(tds);
  }

  // DESTRUCTOR

  ~CGAL_Triangulation_data_structure_3()
  {
    clear();
  }
  
  // ASSIGNEMENT

  inline
  Tds & operator= (const Tds & tds)
  {
    copy_tds(tds);
    return *this;
  }  
    
  // ACCESS FUNCTIONS

  inline 
  int number_of_vertices() const {return _number_of_vertices;}
  
  inline 
  int dimension() const {return _dimension;}

  inline 
  int number_of_cells() const 
    { 
      CGAL_triangulation_precondition( dimension() == 3 );
      int i=0;
      Cell_iterator it = cells_begin();
      while(it != cells_end()) {
	++i;
	++it;
      }
      return i;
    }
  
  inline 
  int number_of_facets() const
    {
      int i=0;
      CGAL_triangulation_precondition( dimension() >= 2 );
      Facet_iterator it = facets_begin();
      while(it != facets_end()) {
	++i;
	++it;
      }
      return i;
    }

  inline 
  int number_of_edges() const
    {
      int i=0;
      CGAL_triangulation_precondition( dimension() >= 1 );
      Edge_iterator it = edges_begin();
      while(it != edges_end()) {
	++i;
	++it;
      }
      return i;
    }

  // USEFUL CONSTANT TIME FUNCTIONS


  // SETTING
  // to be protected ?

  inline 
  void set_number_of_vertices(int n) { _number_of_vertices = n; }

  inline 
  void set_dimension(int n) { _dimension = n; }

  // QUERIES

  bool is_vertex(Vertex* v) const
    {
      Vertex_iterator it = vertices_begin();
      while (it != vertices_end()) {
	if ( v == &(*it) ) return true;
	++it;
      }
      return false;
    }

  bool is_edge(Vertex* u, Vertex* v, 
	       Cell* & c, int & i, int & j) const
    {
      Edge_iterator it = edges_begin();
      while ( it != edges_end() ) {
	if ( ( ((*it).first)->has_vertex(u,i) )
	     && ( ((*it).first)->has_vertex(v,j) ) ) {
	  c = (*it).first;
	  return true;
	}
      }
//       Vertex_iterator it = vertices_begin();
//       // looks at all vertices to find u
//       while (it != vertices_end()) {
// 	if ( u == &(*it) ) {
// 	  // looks at all incident cells to find v
// 	  Cell_circulator cir = incident_cells(u);
// 	  Cell_circulator done = cir;
// 	  while ( cir != done ) {
// 	    if ( v == &(*cir) ) {
// 	      c = &(*cir);
// 	      i = c->index(u);
// 	      j = c->index(v);
// 	      return true;
// 	    }
// 	    ++cir;
// 	  }
// 	  return false;
// 	}
//       }
      return false;
    }
  
  bool is_facet(Vertex* u, Vertex* v, Vertex* w, 
		Cell* & c, int & i, int & j, int & k) const
    {
      Facet_iterator it = facets_begin();
      while ( it != facets_end() ) {
	if ( ( ((*it).first)->has_vertex(u,i) )
	     && ( ((*it).first)->has_vertex(v,j) )
	     && ( ((*it).first)->has_vertex(w,k) ) ) {
	  c = (*it).first;
	  return true;
	}
      }
      return false;
    }
      
  bool is_cell(Vertex* u, Vertex* v, Vertex* w, Vertex* t, 
		Cell* & c, int & i, int & j, int & k, int & l) const
    {
      Cell_iterator it = cells_begin();
      while ( it != cells_end() ) {
	if ( ( it->has_vertex(u,i) )
	     && ( it->has_vertex(v,j) )
	     && ( it->has_vertex(w,k) ) 
	     && ( it->has_vertex(t,l) ) ) {
	  c = &(*it);
	  return true;
	}
      }
      return false;
    }

  // MODIFY

//    void flip(Cell* f, int i)
//     {
//    }

  //INSERTION

  Vertex * insert_in_cell(const Vertex & w, Cell* c)

  { //insert in cell
    CGAL_triangulation_precondition( (c != NULL));

    Vertex * v = new Vertex(w);

//     c->insert_in_cell(v);

    Vertex* v0 = c->vertex(0);
    Vertex* v1 = c->vertex(1);
    Vertex* v2 = c->vertex(2);
    Vertex* v3 = c->vertex(3);

    Cell* n1 = c->neighbor(1);
    Cell* n2 = c->neighbor(2);
    Cell* n3 = c->neighbor(3);

    // c will be modified to have v,v1,v2,v3 as vertices
    Cell* c3 = new Cell(*this,v0,v1,v2,v,c,NULL,NULL,n3);
    Cell* c2 = new Cell(*this,v0,v1,v,v3,c,NULL,n2,c3);
    Cell* c1 = new Cell(*this,v0,v,v2,v3,c,n1,c2,c3);

    c3->set_neighbor(1,c1);
    c3->set_neighbor(2,c2);
    c2->set_neighbor(1,c1);

    n1->set_neighbor(n1->index(c),c1);
    n2->set_neighbor(n2->index(c),c2);
    n3->set_neighbor(n3->index(c),c3);

    c->set_vertex(0,v);
    c->set_neighbor(1,c1);
    c->set_neighbor(2,c2);
    c->set_neighbor(3,c3);

    if( v0->cell() == c  ) {  v0->set_cell(c1); }
    v->set_cell(c);
    set_number_of_vertices(number_of_vertices() +1);

    return v;
  }
  // end insert_in_cell

  Vertex * insert_in_facet(const Vertex & w, const Facet & f)
    {
      return insert_in_facet(w,f.first,f.second);
    }

  Vertex * insert_in_facet(const Vertex & w, Cell* c, int i)
  { // inserts v in the facet opposite to vertex i of cell c

    CGAL_triangulation_precondition( (c != NULL)); 
    CGAL_triangulation_precondition( dimension() >= 2 );

    Vertex * v = new Vertex(w);

    switch ( dimension() ) {

    case 2:
      {
	CGAL_triangulation_precondition( i == 3 );
	Cell* n = c->neighbor(2);
	Cell* cnew = new Cell(*this,
			      c->vertex(0),c->vertex(1),v,NULL,
			      c, NULL,n,NULL);
	n->set_neighbor(n->index(c),cnew);
	c->set_neighbor(2,cnew);
	c->vertex(0)->set_cell(cnew);

	n = c->neighbor(1);
	Cell* dnew = new Cell(*this,
			      c->vertex(0),v,c->vertex(2),NULL,
			      c,n,cnew,NULL);
	n->set_neighbor(n->index(c),dnew);
	c->set_neighbor(1,dnew);
	cnew->set_neighbor(1,dnew);

	c->set_vertex(0,v);
	v->set_cell(c);
	break;
      }
    case 3:
      {
	CGAL_triangulation_precondition( i == 0 || i == 1 || i == 2 || i == 3 );
	// c will be modified to have v replacing vertex(i+3)
	int i1,i2,i3;

	if ( (i&1) == 0 ) {
	  i1=(i+1)&3; i2=(i+2)&3; i3=6-i-i1-i2;
	}
	else {
	  i1=(i+1)&3; i2=(i+3)&3; i3=6-i-i1-i2;
	}
	// i,i1,i2,i3 is well oriented
	// so v will "replace" the vertices in this order
	// when creating the new cells one after another from c

	Vertex* vi=c->vertex(i);
	Vertex* v1=c->vertex(i1); 
	Vertex* v2=c->vertex(i2);
	Vertex* v3=c->vertex(i3);

	// new cell with v in place of i1
	Cell* nc = c->neighbor(i1);
	Cell* cnew1 = new Cell(*this,
			       vi,v,v2,v3,
			       NULL,nc,NULL,c);
	nc->set_neighbor(nc->index(c),cnew1);
	c->set_neighbor(i1,cnew1);

	v3->set_cell(cnew1);

	// new cell with v in place of i2
	nc = c->neighbor(i2);
	Cell* cnew2 = new Cell(*this,
			       vi,v1,v,v3,
			       NULL,cnew1,nc,c);
	nc->set_neighbor(nc->index(c),cnew2);
	c->set_neighbor(i2,cnew2);
	cnew1->set_neighbor(2,cnew2); // links to previous cell

	// v replaces i3 in c
	c->set_vertex(i3,v);

	// other side of facet containing v
	Cell* d = c->neighbor(i);
	int j = d->index(c);
	int j1=d->index(v1);// triangulation supposed to be valid
	int j2=d->index(v2);
	int j3=6-j-j1-j2;
	// then the orientation of j,j1,j2,j3 depends on the parity
	// of i-j

//	if ( (j-i)%2 == 0 ) { IDIOT !!!
	  // new cell with v in place of j1
	  Cell* nd = d->neighbor(j1);
	  Cell* dnew1 = new Cell(*this,
				 d->vertex(j),v,v3,v2,
				 cnew1,nd,d,NULL);
	  nd->set_neighbor(nd->index(d),dnew1);
	  d->set_neighbor(j1,dnew1);
	  cnew1->set_neighbor(0,dnew1);
	  
	  // new cell with v in place of j2
	  nd = d->neighbor(j2);
	  Cell* dnew2 = new Cell(*this,
				 d->vertex(j),v1,v3,v,
				 cnew2,dnew1,d,nd);
	  nd->set_neighbor(nd->index(d),dnew2);
	  d->set_neighbor(j2,dnew2);
	  cnew2->set_neighbor(0,dnew2);
	  dnew1->set_neighbor(3,dnew2);
// 	}
// 	else { IDIOT !!!
// 	  // new cell with v in place of j1
// 	  Cell* nd = d->neighbor(j1);
// 	  Cell* dnew1 = new Cell(*this,
// 			  d->vertex(j),v,v2,v3,
// 			  cnew1,nd,NULL,d);
// 	  nd->set_neighbor(nd->index(d),dnew1);
// 	  d->set_neighbor(j1,dnew1);
// 	  cnew1->set_neighbor(0,dnew1);

// 	  // new cell with v in place of j2
// 	  nd = d->neighbor(j2);
// 	  Cell* dnew2 = new Cell(*this,
// 			  d->vertex(j),v1,v,v3,
// 			  cnew2,dnew1,d,nd);
// 	  nd->set_neighbor(nd->index(d),dnew2);
// 	  d->set_neighbor(j2,dnew2);
// 	  cnew2->set_neighbor(0,dnew2);
// 	  dnew1->set_neighbor(2,dnew2);
// 	}

	// v replaces i3 in d
	d->set_vertex(j3,v);
	v->set_cell(d);

	break;
      }
    }
    set_number_of_vertices(number_of_vertices() +1);

    return v;
  }
  // end insert_in_facet

  Vertex * insert_in_edge(const Vertex & w, const Edge & e)   
  { 
    return insert_in_edge(w, e.first, e.second, e.third);
  }

  Vertex * insert_in_edge(const Vertex & w, Cell* c, int i, int j)   
  { // inserts v in the edge of cell c with vertices i and j
    CGAL_triangulation_precondition( c != NULL ); 
    CGAL_triangulation_precondition( i != j );
    CGAL_triangulation_precondition( dimension() >= 1 );

    Vertex * v = new Vertex(w);

    Cell* cnew;
    Cell* dnew;

    switch ( dimension() ) {

    case 1:
      {
	CGAL_triangulation_precondition( (i==0 || i==1) && (j==0 || j==1) );
	cnew = new Cell(*this,
			v,c->vertex(1),NULL,NULL,
			c->neighbor(0),c,NULL,NULL);
	c->vertex(1)->set_cell(cnew);
	c->set_vertex(1,v);
	c->neighbor(0)->set_neighbor(1,cnew);
	c->set_neighbor(0,cnew);

	v->set_cell(cnew); 
	break;
      }

    case 2:
      {
	CGAL_triangulation_precondition( i>=0 && i<=2 && j>=0 && j<=2 );
	int k=3-i-j; // index of the third vertex of the facet
	Cell* d = c->neighbor(k);
	int kd = d->index(c);
	int id = d->index(c->vertex(i));
	int jd = d->index(c->vertex(j));

	cnew = new Cell(*this);
	cnew->set_vertex(i,c->vertex(i)); 
	c->vertex(i)->set_cell(cnew);
	cnew->set_vertex(j,v);
	cnew->set_vertex(k,c->vertex(k));
	c->set_vertex(i,v);

	dnew = new Cell(*this);
	dnew->set_vertex(id,d->vertex(id));
	// d->vertex(id)->cell() is cnew OK
	dnew->set_vertex(jd,v);
	dnew->set_vertex(kd,d->vertex(kd));
	d->set_vertex(id,v);

	cnew->set_neighbor(i,c);
	Cell* nj = c->neighbor(j);
	cnew->set_neighbor(j,nj);
	nj->set_neighbor(nj->index(c),cnew);
	c->set_neighbor(j,cnew);
	cnew->set_neighbor(k,dnew);

	dnew->set_neighbor(id,d);
	nj = d->neighbor(jd);
	dnew->set_neighbor(jd,nj);
	nj->set_neighbor(nj->index(d),dnew);
	d->set_neighbor(jd,dnew);
	dnew->set_neighbor(kd,cnew);

	v->set_cell(cnew);
	break;
      }

    case 3:
      {
	CGAL_triangulation_precondition( i>=0 && i<=3 && j>=0 && j<=3 );
	Vertex* vi=c->vertex(i);
	Vertex* vj=c->vertex(j);
	
	cnew = new Cell(*this, c);
	c->set_vertex(j,v);
	vj->set_cell(cnew);
	v->set_cell(c);
	c->neighbor(i)->set_neighbor(c->neighbor(i)->index(c),cnew);
	c->set_neighbor(i,cnew);
	cnew->set_vertex(i,v);
	cnew->set_neighbor(j,c);

	// the code here duplicates a large part of the code 
	// of CGAL_Triangulation_ds_cell_circulator_3

	//	int k=Cell_circulator::other(i,j);
	//	Cell* ctmp = c->neighbor(k);
	Cell* ctmp = c->neighbor( nextposaround(i,j) );

	Cell* cprev = c;
	Cell* cnewprev = cnew;

	while ( ctmp != c ) {
	  // the current cell is duplicated. vertices and neighbors i and j
	  // are updated during the traversal.
	  // uses the field prev of the circulator
	  i = ctmp->index(vi);
	  j = ctmp->index(vj);
	  cnew = new Cell(*this, ctmp);
	  // v will become vertex j of c
	  // and vertex i of cnew
	  ctmp->set_vertex(j,v);
	  ctmp->neighbor(i)->set_neighbor(ctmp->neighbor(i)->index(ctmp),cnew);
	  ctmp->set_neighbor(i,cnew);
	  cnew->set_vertex(i,v);
	  cnew->set_neighbor(j,ctmp);

	  // neighbor relations of all cells are used
	  // to find relations between new cells
	  cnew->set_neighbor(ctmp->index(cprev),cnewprev);
	  cnewprev->set_neighbor(cprev->index(ctmp),cnew);

	  cnewprev = cnew;
	  //	  k=Cell_circulator::other(i,j);
	  //	  if ( ctmp->neighbor(k) == cprev ) {
	  //	    cprev = ctmp;
	  //	    ctmp = ctmp->neighbor(6-i-j-k);
	  //	  }
	  //	  else {
	  //	    cprev = ctmp;
	  //	    ctmp = ctmp->neighbor(k);
	  //	  }
	  cprev = ctmp;
	  ctmp = ctmp->neighbor( nextposaround(i,j) );
	}
	cnew = c->neighbor(c->index(vi));
	cnew->set_neighbor(c->index(cprev),cnewprev);
	cnewprev->set_neighbor(cprev->index(c),cnew);
	break;
      }
    }
    set_number_of_vertices(number_of_vertices() +1);

    return v;
  }// end insert_in_edge


  Vertex * insert_outside_affine_hull(const Vertex & w, // new vertex
				  Vertex* star = NULL,
				  bool reorient = false) 
    // star = vertex from which we triangulate the facet of the incremented dimension
    // ( geometrically : star = infinite vertex )
    // = Null only used to insert the 1st vertex (dimension -2 to dimension -1)

    // changes the dimension

    // if (reorient) the orientation of the cells is modified
  {  // insert()
    Vertex * v = new Vertex(w);

    Cell* c;
    Cell* d;
    Cell* e;
    int i, j;

    switch ( dimension() ) {

    case -2:
      // insertion of the first vertex
      // ( geometrically : infinite vertex )
      {
	set_number_of_vertices( number_of_vertices()+1 );
	set_dimension( dimension()+1 );

	c = new Cell( *this,
		      v, NULL, NULL, NULL,
		      NULL, NULL, NULL, NULL );
	v->set_cell(c);
	break;
      }

    case -1:
      // insertion of the second vertex
      // ( geometrically : first finite vertex )
      {
	CGAL_triangulation_precondition( star != NULL );
	CGAL_triangulation_precondition( is_vertex(star) );

	set_number_of_vertices( number_of_vertices()+1 );
	set_dimension( dimension()+1 );

	d = new Cell( *this,
		      v, NULL, NULL, NULL,
		      star->cell(), NULL, NULL, NULL );
	v->set_cell(d);
	star->cell()->set_neighbor(0,d);
	break;
      }

    case 0:
      // insertion of the third vertex
      // ( geometrically : second finite vertex )
      {
	CGAL_triangulation_precondition( star != NULL );
	CGAL_triangulation_precondition( is_vertex(star) );

	set_number_of_vertices( number_of_vertices()+1 );
	set_dimension( dimension()+1 );

	c = star->cell();
	d = c->neighbor(0);
	// the following code could be shortened :
	// if (reorient) { i=0; j=1 }
	// else { i=1; j=0 }
	// and then use i and j instead of 0 and 1
	if (reorient) {
	  c->set_vertex(0,d->vertex(0));
	  c->set_vertex(1,star);
	  c->set_neighbor(1,d);
	  d->set_vertex(1,d->vertex(0));
	  d->set_vertex(0,v);
	  d->set_neighbor(0,c);
	  e = new Cell( *this,
			star, v, NULL, NULL,
			d, c, NULL, NULL );
	  c->set_neighbor(0,e);
	  d->set_neighbor(1,e);
	}
	else {
	  c->set_vertex(1,d->vertex(0));
	  d->set_vertex(1,v);
	  d->set_neighbor(1,c);
	  e = new Cell( *this,
			v, star, NULL, NULL,
			c, d, NULL, NULL );
	  c->set_neighbor(1,e);
	  d->set_neighbor(0,e);
	}
	
	v->set_cell(d);
	break;
      }

    case 1:
      // general case : 4th vertex ( geometrically : 3rd finite vertex )
      // degenerate cases geometrically : 1st non collinear vertex
      {
	CGAL_triangulation_precondition( star != NULL );
	CGAL_triangulation_precondition( is_vertex(star) );

	set_number_of_vertices( number_of_vertices()+1 );
	set_dimension( dimension()+1 );
	// this is set now, so that it becomes allowed to reorient
	// new facets or cells by iterating on them (otherwise the
	// dimension is to small)

	c = star->cell();
	i = c->index(star); // i== 0 or 1
	j = (1-i);
	d = c->neighbor(j);
	
	c->set_vertex(2,v);

	e = c->neighbor(i);
	Cell* cnew = c;
	Cell* enew;
	
	while( e != d ){
	  enew = new Cell( *this );
	  enew->set_vertex(i,e->vertex(j));
	  enew->set_vertex(j,e->vertex(i));
	  enew->set_vertex(2,star);
	  
	  enew->set_neighbor(i,cnew);
	  cnew->set_neighbor(j,enew); 
	  // false at the first iteration of the loop where it should be neighbor 2
	  // it is corrected after the loop
	  enew->set_neighbor(2,e);
	  // neighbor j will be set during next iteration of the loop
	  
	  e->set_vertex(2,v);
	  e->set_neighbor(2,enew);
	  
	  c = e;
	  e = e->neighbor(i);
	  cnew = enew;
	}
	
	d->set_vertex(2,v);
	d->set_neighbor(2,enew);
	enew->set_neighbor(j,d);
	
	// corrections for star->cell() :
	c = star->cell();
	c->set_neighbor(2,c->neighbor(i)->neighbor(2));
	c->set_neighbor(j,d);
	
	v->set_cell(d);
	
	if (reorient) {
	  // reorientation of all the cells
	  Vertex* vtmp;
	  Cell* ctmp;
	  Facet_iterator fit = facets_begin();
	  
	  while(fit != facets_end()) {
	    vtmp = (*fit).first->vertex(1);
	    (*fit).first->set_vertex(1,(*fit).first->vertex(0));
	    (*fit).first->set_vertex(0,vtmp);
	    
	    ctmp = (*fit).first->neighbor(1);
	    (*fit).first->set_neighbor(1,(*fit).first->neighbor(0));
	    (*fit).first->set_neighbor(0,ctmp);
	    
	    ++fit;
	  }
	}
	break;
      }

    case 2:
      // general case : 5th vertex ( geometrically : 4th finite vertex )
      // degenerate cases : geometrically 1st non coplanar vertex
      {
	CGAL_triangulation_precondition( star != NULL );
	CGAL_triangulation_precondition( is_vertex(star) );

	set_number_of_vertices( number_of_vertices()+1 );
	set_dimension( dimension()+1 );

	Cell* old_cells = list_of_cells()._next_cell; 
	// used to store the beginning of the list of cells,
	// which will be past end for the list of new cell
	// in order to be able to traverse only the new cells 
	// to find the missing neighbors (we know that new Cell() puts
	// each new cell at the beginning of the list).
	
	Cell* cnew;
	Cell_iterator it = cells_begin(); 
	// allowed since the dimension has already been set to 3

	v->set_cell(&(*it)); // ok since there is at list one ``cell''
	while (it != cells_end()) {
	  it->set_vertex(3,v);
	  if ( ! it->has_vertex(star) ) {
	    cnew = new Cell( *this,
			     it->vertex(0),it->vertex(2),
			     it->vertex(1),star,
			     NULL,NULL,NULL,&(*it));
	    it->set_neighbor(3,cnew);
	  }
	  ++it;
	}

	it = cells_begin(); 
	Cell* n;
	Cell* c;
	// traversal of the new cells only, to add missing neighbors
	while ( &(*it) != old_cells ) {
	  n = it->neighbor(3); // opposite to star
	  for ( int i=0; i<3; i++ ) {
	    int j;
	    if ( i==0 ) j=0;
	    else j=3-i; // vertex 1 and vertex 2 are always switched when
	                // creating a new cell (see above)
	    if ( ( c = n->neighbor(i)->neighbor(3) ) != NULL ) {
	      // i.e. star is not a vertex of n->neighbor(i)
	      it->set_neighbor(j,c);
	      // opposite relation will be set when it arrives on c
	      // this avoids to look for the correct index 
	      // and to test whether *it already has neighbor i
	    }
	    else {
	      // star is a vertex of n->neighbor(i)
	      it->set_neighbor(j,n->neighbor(i));
	      n->neighbor(i)->set_neighbor(3,&(*it)); // neighbor opposite to v
	    }
	  }
	  ++it;
	}
	
	// reorientation of all the cells
	if (reorient) {
	  Vertex* vtmp;
	  Cell* ctmp;
	  it = cells_begin();
	  
	  while ( it != cells_end() ) {
	    vtmp = it->vertex(1);
	    it->set_vertex(1,it->vertex(0));
	    it->set_vertex(0,vtmp);

	    ctmp = it->neighbor(1);
	    it->set_neighbor(1,it->neighbor(0));
	    it->set_neighbor(0,ctmp);
	    
	    ++it;
	  }
	}
      }
    }// end switch
    
    return v;
  } // end insert_outside_affine_hull

  // for Delaunay :
  void star_region( set<void*, less<void*> > & region, Vertex* v,
		     Cell* c, int li )
    // region is a set of connected cells
    // c belongs to region and has facet i on the boundary of region 
    // replaces the cells in region  
    // by linking v to the boundary of region 
  {
    CGAL_triangulation_precondition( dimension() >= 2 );
    CGAL_triangulation_precondition( region.find( (void *) c )  
				     != region.end() );
    // does not check whether region is connected 
    Cell* nouv = create_star( region, v, c, li );
    v->set_cell( nouv );
    // v->set_cell( create_star( region, v, c, li ) );
    set<void*, less<void*> >::const_iterator it;
    for( it = region.begin(); it != region.end(); ++it) {
      delete( (Cell *) *it);
    }
  }
    
private:
  Cell* create_star( set<void*, less<void*> > & region, Vertex* v,
		     Cell* c, int li )
    // creates the cells needed by star_region
  {
    Cell* cnew;
    if ( dimension() == 3 ) {
      unsigned int i[3];
      if ( (li%2) == 1 ) {
	i[0] = (li+1)&3;
	i[1] = (li+2)&3;
	i[2] = (li+3)&3;
      }
      else {
	i[0] = (li+2)&3;
	i[1] = (li+1)&3;
	i[2] = (li+3)&3;
      }
      cnew = new Cell( *this,
		       c->vertex(i[0]), c->vertex(i[1]), c->vertex(i[2]), v,
		       NULL, NULL, NULL, c->neighbor(li) );
      c->neighbor(li)->set_neighbor( c->neighbor(li)->index(c), cnew);

      // look for the other three neighbors of cnew
      int j1, j2;
      Cell* cur;
      Cell* n;
      for (int ii=0; ii<3; ii++) {
	cnew->vertex(ii)->set_cell(cnew);
	// indices of the vertices of cnew such that i[ii],j1,j2,li positive
	j1 = nextposaround(i[ii],li);
	j2 = 6-i[ii]-li-j1;
	// turn around the oriented edge j1 j2
	cur = c;
	n = c->neighbor(i[ii]);
	CGAL_triangulation_assertion( nextposaround(j1,j2)==i[ii] );//debug
	while (true) {
	  j1 = n->index( cur->vertex(j1) );
	  j2 = n->index( cur->vertex(j2) );
	  if ( region.find( (void*) n ) == region.end() ) { 
	    //not in conflict
            break;
	  }
	  CGAL_triangulation_assertion( n != c );
	  cur = n;
	  n = n->neighbor( nextposaround(j1,j2) );
	}
	// now n is outside region, cur is inside
	if ( n->neighbor( nextposaround(j2,j1) ) == cur ) {
	  // neighbor relation is reciprocical, ie
	  // the cell we are looking for is not yet created
	  cnew->set_neighbor(ii,create_star(region,v,cur,cur->index(n)));
	  continue;
	}
	// else the cell we are looking for was already created
	cnew->set_neighbor(ii,n->neighbor( nextposaround(j2,j1) ));
      }
      return cnew;
    } // endif dimension 3
    
    // else dimension 2 
    // i1 i2 such that v,i1,i2 positive
    int i1=ccw(li);
/*     cnew = new Cell( *this, */
/* 		     v, c->vertex(i1), c->vertex(i2), NULL, */
/* 		     c->neighbor(li), NULL, NULL, NULL); */
/*     c->vertex(i1)->set_cell(cnew); */
/*     c->neighbor(li)->set_neighbor( c->neighbor(li)->index(c), cnew); */
    // traversal of the boundary of region in ccw order to create all
    // the new facets
    Cell* bound = c;
    Vertex* v1 = c->vertex(i1);
    int ind = c->neighbor(li)->index(c); // to be able to find the
    // first cell that will be created 
    Cell* cur;
    Cell* pnew = NULL;
    do {
      cur = bound;
      // turn around v2 until we reach the boundary of region
      while ( region.find( (void*) cur->neighbor(cw(i1)) ) !=
	      region.end() ) {
	// neighbor in conflict
	cur = cur->neighbor(cw(i1));
	i1 = cur->index( v1 );
      }
      // here cur has an edge on the boundary of region
      cnew = new Cell( *this,
		       v, v1, cur->vertex( ccw(i1) ), NULL,
		       cur->neighbor(cw(i1)), NULL, pnew, NULL);
      cur->neighbor(cw(i1))->set_neighbor
	( cur->neighbor(cw(i1))->index(cur), cnew );
      // pnew is null at the first iteration
      v1->set_cell(cnew);
      //pnew->set_neighbor( cw(pnew->index(v1)), cnew );
      if (pnew) { pnew->set_neighbor( 1, cnew );}

      bound = cur;
      i1 = ccw(i1);
      v1 = bound->vertex(i1);
      pnew = cnew;
      //} while ( ( bound != c ) || ( li != cw(i1) ) );
    } while ( v1 != c->vertex(ccw(li)) );
    // missing neighbors between the first and the last created cells
    cur = c->neighbor(li)->neighbor(ind); // first created cell
    cnew->set_neighbor( 1, cur );
    cur->set_neighbor( 2, cnew );
    return cnew;
  }
public:

  // ITERATOR METHODS

  Cell_iterator cells_begin() const
  {
    //    CGAL_triangulation_precondition( dimension() == 3 );
    if ( dimension() < 3 ) return cells_end();
    Tds* ncthis = (Tds *)this;
    return Cell_iterator(ncthis);
  }

  Cell_iterator cells_end() const
  {
    //    CGAL_triangulation_precondition( dimension() == 3 );
    Tds* ncthis = (Tds *)this;
    return Cell_iterator(ncthis, 1);
  }

  Facet_iterator facets_begin() const
  {
    if ( dimension() < 2 ) return facets_end();
    //    CGAL_triangulation_precondition( dimension() >=2 );
    Tds* ncthis = (Tds*)this;
    return Facet_iterator(ncthis);
  }

  Facet_iterator facets_end() const
  {
    //    CGAL_triangulation_precondition( dimension() >=2 );
    Tds* ncthis = (Tds*)this;
    return Facet_iterator(ncthis,1);
  }

  Edge_iterator edges_begin() const
  {
    if ( dimension() < 1 ) return edges_end();
    //    CGAL_triangulation_precondition( dimension() >=1 );
    Tds* ncthis = (Tds*)this;
    return Edge_iterator(ncthis);
  }

  Edge_iterator edges_end() const
  {
    //    CGAL_triangulation_precondition( dimension() >=1 );
    Tds* ncthis = (Tds*)this;
    return Edge_iterator(ncthis,1);
  }

  Vertex_iterator vertices_begin() const
  {
    if ( number_of_vertices() <= 0 ) return vertices_end();
//    CGAL_triangulation_precondition( number_of_vertices() > 0 );
    Tds* ncthis = (Tds*)this;
    return Vertex_iterator(ncthis);
  }

  Vertex_iterator vertices_end() const
  {
    //    CGAL_triangulation_precondition( number_of_vertices() > 0 );
    Tds* ncthis = (Tds*)this;
    return Vertex_iterator(ncthis,1);
  }

  // CIRCULATOR METHODS

  Cell_circulator incident_cells(const Edge & e) const
  {
    CGAL_triangulation_precondition( dimension() == 3 );
    Tds* ncthis = (Tds *)this;
    return Cell_circulator(ncthis,e);
  }
  Cell_circulator incident_cells(Cell* ce, int i) const
  {
    return incident_cells(make_pair(ce,i));
  }

  Cell_circulator incident_cells(const Edge & e, Cell* c) const
  {
    CGAL_triangulation_precondition( dimension() == 3 );
    Tds* ncthis = (Tds *)this;
    return Cell_circulator(ncthis,e,c);
  }
  Cell_circulator incident_cells(Cell* ce, int i, Cell* c) const
  {
    return incident_cells(make_pair(ce,i),c);
  }

  void
  incident_cells(Vertex* v, set<Cell*, less<Cell*> > & cells,
		 Cell* c = NULL ) const
    {
      CGAL_triangulation_precondition( v != NULL );
      CGAL_triangulation_precondition( is_vertex(v) );

      if ( dimension() < 3 ) { return; }

      if ( c == NULL ) {
	c = v->cell();
      }
      else {
	CGAL_triangulation_precondition( c->has_vertex(v) );
      }
      if ( cells.find( c ) != cells.end() ) {
	return; // c was already found
      }
      cells.insert( c );
      
      for ( int j=0; j<4; j++ ) {
	if ( j != c->index(v) ) {
	  incident_cells( v, cells, c->neighbor(j) );
	}
      }
    }
  
  void
  incident_vertices(Vertex* v, set<Vertex*, less<Vertex*> > & vertices,
		    Cell* c = NULL ) const
    {
      CGAL_triangulation_precondition( v != NULL );
      CGAL_triangulation_precondition( is_vertex(v) );
      
      if ( number_of_vertices() < 2 ) { return; }

      if ( c == NULL ) {
	c = v->cell();
      }
      else {
	CGAL_triangulation_precondition( c->has_vertex(v) );
      }

      int d = dimension();
      int j;
      int found = 0;
      for ( j=0; j <= d; j++ ) {
	if ( j != c->index(v) ) {
	  if ( vertices.find( c->vertex(j) ) == vertices.end() ) {
	    vertices.insert( c->vertex(j) );
	  }
	  else {
	    found++; // c->vertex(j) was already found 
	  }
	}
      }
      if ( found == 3 ) return; // c was already visited
      
      for ( j=0; j <= d; j++ ) {
	if ( j != c->index(v) ) {
	  incident_vertices( v, vertices, c->neighbor(j) );
	}
      }
    }

  // CHECKING
  bool is_valid(bool verbose = false, int level = 0) const
  { // is_valid()
    switch ( dimension() ) {
    case 3:
      {
	int vertex_count;
	if ( ! count_vertices(vertex_count,verbose,level) ) {return false;}
	if ( number_of_vertices() != vertex_count ) {
	  if (verbose) { cerr << "false number of vertices" << endl; }
	  CGAL_triangulation_assertion(false); return false;
	}

	int edge_count;
	if ( ! count_edges(edge_count,verbose,level) ) {return false;}
	int facet_count;
	if ( ! count_facets(facet_count,verbose,level) ) {return false;}
	int cell_count;
	if ( ! count_cells(cell_count,verbose,level) ) {return false;}

	// Euler relation 
	if ( cell_count - facet_count + edge_count - vertex_count != 0 ) {
	  if (verbose) { cerr << "Euler relation unsatisfied"<< endl; }
	  CGAL_triangulation_assertion(false); return false;
	}

	break;
      }
    case 2:
      {
	int vertex_count;
	if ( ! count_vertices(vertex_count,verbose,level) ) {return false;}
	if ( number_of_vertices() != vertex_count ) {
	  if (verbose) { cerr << "false number of vertices" << endl; }
	  CGAL_triangulation_assertion(false); return false;
	}

	int edge_count;
	if ( ! count_edges(edge_count,verbose,level) ) {return false;}
	// Euler for edges
	if ( edge_count != 3 * vertex_count - 6 ) {
	  if (verbose) { cerr << "Euler relation unsatisfied - edges/vertices" << endl;}
	  CGAL_triangulation_assertion(false); return false;
	}

	int facet_count;
	if ( ! count_facets(facet_count,verbose,level) ) {return false;}
	// Euler for facets
	if ( facet_count != 2 * vertex_count - 4 ) {
	  if (verbose) { cerr << "Euler relation unsatisfied - facets/vertices" << endl;}
	  CGAL_triangulation_assertion(false); return false;
	}
	break;
      }
    case 1:
      {
	int vertex_count;
	if ( ! count_vertices(vertex_count,verbose,level) ) {return false;}
	if ( number_of_vertices() != vertex_count ) {
	  if (verbose) { cerr << "false number of vertices" << endl; }
	  CGAL_triangulation_assertion(false); return false;
	}
	int edge_count;
	if ( ! count_edges(edge_count,verbose,level) ) {return false;}
	// Euler for edges
	if ( edge_count != vertex_count ) {
	  if (verbose) { cerr << "false number of edges" << endl; }
	  CGAL_triangulation_assertion(false); return false;
	}
	break;
      }
    case 0:
      {
	if ( number_of_vertices() < 2 ) {
	  if (verbose) { cerr << "less than 2 vertices but dimension 0" << endl; }
	  CGAL_triangulation_assertion(false); return false;
	}
	// no break; continue
      }
    case -1:
      {
	if ( number_of_vertices() < 1 ) {
	  if (verbose)
	    cerr << "no vertex but dimension -1" << endl;
	  CGAL_triangulation_assertion(false);
	  return false;
	}
	// vertex count
	int vertex_count;
	if ( ! count_vertices(vertex_count,verbose,level) )
	  return false;
	if ( number_of_vertices() != vertex_count ) {
	  if (verbose)
	    cerr << "false number of vertices" << endl;
	  CGAL_triangulation_assertion(false);
	  return false;
	}
      } 
    } // end switch
    if (verbose) { cerr << "valid data structure" << endl; }
    return true;
  } // end is_valid

  //Helping functions

  void init(Vertex*  v)
    {
    }

  Vertex* copy_tds(const Tds & tds, Vertex* vert = NULL)
    // returns the new vertex corresponding to vert in the new tds 
  {
    if ( vert != NULL ) {
      CGAL_triangulation_precondition( tds.is_vertex(vert) );
    }

    map< void*, void*, less<void*> > V;
    map< void*, void*, less<void*> > F;
    Vertex*  v;
    Cell* f;

    clear();

    int n = tds.number_of_vertices();
    set_number_of_vertices(n);
    set_dimension(tds.dimension());

    if(n == 0){ return vert; }

    { // create the vertices

      Vertex_iterator it=tds.vertices_begin();
      while (it != tds.vertices_end()) {
	V[&(*it)] = new Vertex( it->point() );
	++it;
      }
    }

    { // create the cells
      Cell* it = tds._list_of_cells._next_cell;
      while ( it != tds.past_end_cell() ){
	F[&(*it)]=  new Cell( *this,
			      (Vertex*) V[it->vertex(0)],
			      (Vertex*) V[it->vertex(1)],
			      (Vertex*) V[it->vertex(2)],
			      (Vertex*) V[it->vertex(3)]);
	it = it->_next_cell;
      }
    }

//    only works in dimension 3
//     { // create the cells
//       Cell_iterator it = tds.cells_begin();
//       while(it != tds.cells_end()){
// 	F[&(*it)]=  new Cell( *this,
// 			      (Vertex*) V[it->vertex(0)],
// 			      (Vertex*) V[it->vertex(1)],
// 			      (Vertex*) V[it->vertex(2)],
// 			      (Vertex*) V[it->vertex(3)]);
// 	++(it);
//       }
//     }

    { // link the vertices to a cell
      Vertex_iterator it = tds.vertices_begin();
      while(it != tds.vertices_end()) {
            v = (Vertex*) V[&(*it)];
            v->set_cell( (Cell*) F[it->cell()] );
            ++it;
        }
    }

    { // hook neighbor pointers of the cells
      Cell* it = tds._list_of_cells._next_cell;
      while ( it != tds.past_end_cell() ){
	for(int j = 0; j < 4; j++){
            f = ((Cell*) F[&(*it)]);
            f->set_neighbor(j, (Cell*) F[it->neighbor(j)] );
          }
	it = it->_next_cell;
      }
    }

//     only works in dimension 3
//     { // hook neighbor pointers of the cells
//       Cell_iterator it = tds.cells_begin();
//       while(it != tds.cells_end()){
//           for(int j = 0; j < 3; j++){
//             f = ((Cell*) F[&(*it)]);
//             f->set_neighbor(j, (Cell*) F[it->neighbor(j)] );
//           }
//           ++it;
//         }
//     }

    CGAL_triangulation_postcondition( is_valid(true) );
    if ( vert != NULL ) {
      return (Vertex*) V[vert];
    }
    else return NULL;
  }
 
  
  void swap(Tds & tds)
  {
    // tds and *this are supposed to be valid
    int dim = dimension();
    int nb = number_of_vertices();
    Cell *l = list_of_cells()._next_cell;
    Cell* p = list_of_cells()._previous_cell;

    set_dimension(tds.dimension());
    set_number_of_vertices(tds.number_of_vertices());

    if ( tds.list_of_cells()._next_cell == &(tds.list_of_cells()) ) {
      list_of_cells()._next_cell = list_of_cells()._previous_cell 
	= &( list_of_cells() );
    }
    else {
      _list_of_cells._next_cell = tds.list_of_cells()._next_cell;
      _list_of_cells._next_cell->_previous_cell = &(_list_of_cells);
      _list_of_cells._previous_cell = tds.list_of_cells()._previous_cell;
      _list_of_cells._previous_cell->_next_cell = &(_list_of_cells);
    }

    tds._dimension = dim;
    tds._number_of_vertices = nb;

    if ( l == &(list_of_cells()) ) {
      tds._list_of_cells._next_cell = tds._list_of_cells._previous_cell 
	= &( tds._list_of_cells );
    }
    else {
      tds._list_of_cells._next_cell = l;
      tds._list_of_cells._next_cell->_previous_cell = &(tds._list_of_cells);
      tds._list_of_cells._previous_cell = p;
      tds._list_of_cells._previous_cell->_next_cell = &(tds._list_of_cells);
    }
  }

  void clear()
  {

    if(number_of_vertices() == 0) {
      // the list of cells must be cleared even in this case
      Cell* it=_list_of_cells._next_cell;
      while ( it != past_end_cell() ) {
	delete it;
	// uses the destructor of ds_cell, which 
	// removes the cell from the list of cells
	it=_list_of_cells._next_cell;    
      };
      // then _list_of_cells points on itself, nothing more to do
      set_dimension(-2);
      return;
    }

    list<Vertex *> Vertices;
    {// creation of a list of all vertices
      Vertex_iterator it = vertices_begin(), done = vertices_end();
      do{
	Vertices.push_back(&(*it));
      } while(++it!=done);
    }
    
    {// deletion of the cells
      // does not use the cell iterator to work in any dimension
      Cell* it=_list_of_cells._next_cell;
      while ( it != past_end_cell() ) {
	delete it;
	// uses the destructor of ds_cell, which 
	// removes the cell from the list of cells
	it=_list_of_cells._next_cell;    
      };
      // then _list_of_cells points on itself, nothing more to do
    }



    {// deletion of the vertices
      list<Vertex*>::iterator
	it=Vertices.begin(),done=Vertices.end();
      do{
	delete *it;
      } while (++it!=done);
    }

    set_number_of_vertices(0);
    set_dimension(-2);
  }


private:
  // in dimension i, number of vertices >= i+2 
  // ( the boundary of a simplex in dimension i+1 has i+2 vertices )
  int _dimension; // 
  int _number_of_vertices;
  
  // we maintain the list of cells to be able to traverse the triangulation
  // it starts with a "foo" element that will never be removed.
  // the list is circular, the foo element being used to recognize the end
  // of the list
  Cell _list_of_cells;
  
  // ACCESS FUNCTIONS

  inline
  Cell & list_of_cells() 
    {return _list_of_cells;}
  
  inline 
  Cell* past_end_cell() const 
    {
      Tds* ncthis = (Tds *)this;
      return &( ncthis->_list_of_cells );
    } 

  // used by is-valid
  bool count_vertices(int & i, bool verbose = false, int level = 0) const
    // counts AND checks the validity
  {
    i = 0;
    Vertex_iterator it = vertices_begin();
    
    while(it != vertices_end()) {
      if ( ! it->is_valid(verbose,level) ) {
	if (verbose) { cerr << "invalid vertex" << endl; }
	CGAL_triangulation_assertion(false); return false;
      }
      ++i;
      ++it;
    }
    return true;
  } 
  
  bool count_facets(int & i, bool verbose = false, int level = 0) const
    // counts but does not check
  {
    i = 0;
    Facet_iterator it = facets_begin();
    
    while(it != facets_end()) {
      if ( ! (*it).first->is_valid(dimension(),verbose, level) ) {
	if (verbose) { cerr << "invalid facet" << endl;}
	CGAL_triangulation_assertion(false); return false;
      }
      ++i;
      ++it;
    }
    return true;
  }

  bool count_edges(int & i, bool verbose = false, int level = 0) const
    // counts but does not check
  {
    i = 0;
    Edge_iterator it = edges_begin();
    
    while(it != edges_end()) {
      if ( ! (*it).first->is_valid(dimension(),verbose, level) ) {
	if (verbose) { cerr << "invalid edge" << endl;}
	CGAL_triangulation_assertion(false); return false;
      }
      ++i;
      ++it;
    }
    return true;
  }

  bool count_cells(int & i, bool verbose = false, int level = 0) const
    // counts AND checks the validity
  {
    i = 0;
    Cell_iterator it = cells_begin();
    
    while(it != cells_end()) {
      if ( ! it->is_valid(dimension(),verbose, level) ) {
	if (verbose) { cerr << "invalid cell" << endl;}
	CGAL_triangulation_assertion(false); return false;
      }
      ++i;
      ++it;
    }
    return true;
  }
  
};

template < class Vb, class Cb>
istream& operator>>
(istream& is, CGAL_Triangulation_data_structure_3<Vb,Cb>& tds)
  // reads :
  // the dimension
  // the number of vertices
  // the number of cells
  // the cells by the indices of their vertices 
  // the neighbors of each cell by their index in the preceding list of cells
  // when dimension < 3 : the same with faces of maximal dimension
{

  typedef CGAL_Triangulation_data_structure_3<Vb,Cb> Tds;
  typedef  Tds::Vertex  Vertex;
  typedef  Tds::Cell Cell;
  typedef  Tds::Edge Edge;
  typedef  Tds::Facet Facet;
  //  typedef typename Vb::Point Point;

  tds.clear();

  int i;
  int n, d;
  is >> d >> n;
  tds.set_dimension(d);
  tds.set_number_of_vertices(n);

  if(n == 0){
    return is;
  }

  //  Point p;
  map< int, Vertex*, less<int> > V;
  //  vector<Vertex*> V(n);
  
  // creation of the vertices    

  for (i=0; i < n; i++) {
    //    is >> p;
    //    V[i] = new Vertex(p);
    V[i] = new Vertex();
  }

  map< int, Cell*, less<int> > C;
  int m;
//   // creation of the cells and neighbors
//   switch (d) {
//   case 3:
//     {
//       is >> m;
//      vector<Cell*> C(m);
//       Cell* c;

//       int i0, i1, i2, i3;
//       for(i = 0; i < m; i++) {
// 	is >> i0 >> i1 >> i2 >> i3;
// 	c = new Cell(tds, V[i0], V[i1], V[i2], V[i3]);
// 	C[i] = c;
// 	V[i0]->set_cell(c);
// 	V[i1]->set_cell(c);
// 	V[i2]->set_cell(c);
// 	V[i3]->set_cell(c);
//       }
//       for(i = 0; i < m; i++) {
//         is >> i0 >> i1 >> i2 >> i3;
//         c = C[i];
//         c->set_neighbor(0, C[i0]);
//         c->set_neighbor(1, C[i1]);
//         c->set_neighbor(2, C[i2]);
//         c->set_neighbor(3, C[i3]);
//       }
//       break;
//     }
//   case 2:
//     {
//       is >> m;
//      vector<Cell*> C(m);
//       Cell* c;

//       int i0, i1, i2;
//       for(i = 0; i < m; i++) {
// 	is >> i0 >> i1 >> i2;
// 	c = new Cell(tds, V[i0], V[i1], V[i2], NULL);
// 	C[i] = c;
// 	V[i0]->set_cell(c);
// 	V[i1]->set_cell(c);
// 	V[i2]->set_cell(c);
//       }
//       for(i = 0; i < m; i++) {
//         is >> i0 >> i1 >> i2;
// 	c = C[i];
//         c->set_neighbor(0, C[i0]);
//         c->set_neighbor(1, C[i1]);
//         c->set_neighbor(2, C[i2]);
//       }
//       break;
//     }
//   case 1:
//     {
//       is >> m;
//      vector<Cell*> C(m);
//       Cell* c;

//       int i0, i1;
//       for(i = 0; i < m; i++) {
// 	is >> i0 >> i1;
// 	c = new Cell(tds, V[i0], V[i1], NULL, NULL);
// 	C[i] = c;
// 	V[i0]->set_cell(c);
// 	V[i1]->set_cell(c);
//       }
//       for(i = 0; i < m; i++) {
//         is >> i0 >> i1;
// 	c = C[i];
//         c->set_neighbor(0, C[i0]);
//         c->set_neighbor(1, C[i1]);
//       }
//       break;
//     }
//   case 0:
//     {
//       m=2;
//      vector<Cell*> C(2);
//       Cell* c;

//       CGAL_triangulation_assertion( (n == 2) );
//       for (i=0; i < 2; i++) {
// 	c = new Cell(tds, V[i], NULL, NULL, NULL);
// 	C[i] = c;
// 	V[i]->set_cell(c);
//       }
//       for (i=0; i < 2; i++) {
// 	c = C[i];
//         c->set_neighbor(0, C[1-i]);
//       }
//       break;
//     }
//   case -1:
//     {
//       m = 1;
//      vector<Cell*> C(1);
//       Cell* c;
//       CGAL_triangulation_assertion( (n == 1) );
//       c = new Cell(tds, V[0], NULL, NULL, NULL);
//       V[0]->set_cell(c);
//       break;
//     }
//   }
 
  read_cells(is, tds, n, V, m, C);
  CGAL_triangulation_assertion( tds.is_valid(true) );
  return is;
}


template < class Vb, class Cb>
ostream& operator<<
(ostream& os, const CGAL_Triangulation_data_structure_3<Vb,Cb>  &tds)
  // writes :
  // the dimension
  // the number of vertices
  // the number of cells
  // the cells by the indices of their vertices 
  // the neighbors of each cell by their index in the preceding list of cells
  // when dimension < 3 : the same with faces of maximal dimension
{
  typedef CGAL_Triangulation_data_structure_3<Vb,Cb> Tds;
  typedef  Tds::Vertex  Vertex;
  typedef  Tds::Cell Cell;
  typedef  Tds::Edge Edge;
  typedef  Tds::Facet Facet;
  typedef  Tds::Vertex_iterator  Vertex_iterator;
  typedef  Tds::Cell_iterator  Cell_iterator;
  typedef  Tds::Edge_iterator  Edge_iterator;
  typedef  Tds::Facet_iterator  Facet_iterator;

  map< void*, int, less<void*> > V;
  //  map< void*, int, less<void*> > C;

  // outputs dimension and number of vertices
  int n = tds.number_of_vertices();
  switch ( tds.dimension() ) {
  case 3:
    {
      if(CGAL_is_ascii(os)){
        os << tds.dimension() << endl << n << endl;
      } else {
        os << tds.dimension() << n;
      }
      break;
    }
  case 2:
    {
      if(CGAL_is_ascii(os)){
        os << tds.dimension() << endl << n << endl;
      } else {
        os << tds.dimension() << n;
      }
      break;
    }
  case 1:
    {
      if(CGAL_is_ascii(os)){
        os << tds.dimension() << endl << n << endl;
      } else {
        os << tds.dimension() << n ;
      }
      break;
    }
  case 0:
    {
      if(CGAL_is_ascii(os)){
	os << tds.dimension() << endl << n << endl;
      } else {
	os << tds.dimension() << n;
      }
      break;
    }
  default:
    {
      if(CGAL_is_ascii(os)){
	os << tds.dimension() << endl << n << endl;
      } else {
	os << tds.dimension() << n;
      }
    }
  }

  if (n == 0){
    return os;
  }
  
  // index the vertices
  int i = 0;
  Vertex_iterator it = tds.vertices_begin();
    
  while(it != tds.vertices_end()){
    V[&(*it)] = i++;
    //    os << it->point();
    //    if(CGAL_is_ascii(os)){
    //      os << endl;
    //    }
    ++it;
  }
  CGAL_triangulation_assertion( i == n );

  print_cells(os, tds, n, V);

//   i = 0;
//   int j;
//   switch ( tds.dimension() ) {
//   case 3:
//     {
//       os << m;
//       if(CGAL_is_ascii(os)){ os << endl;}

//       // write the cells
//       Cell_iterator it = tds.cells_begin();
//       while( it != tds.cells_end() ) {
// 	C[&(*it)] = i++;
// 	for(j = 0; j < 4; j++){
// 	  os << V[it->vertex(j)];
// 	  if(CGAL_is_ascii(os)) {
// 	    if ( j==3 ) {
// 	      os << endl;
// 	    } else {
// 	      os << ' ';
// 	    }
// 	  }
// 	}
// 	++it;
//       }
//       CGAL_triangulation_assertion( i == m );
      
//       // write the neighbors
//       it = tds.cells_begin();
//       while ( it != tds.cells_end() ) {
// 	for (j = 0; j < 4; j++) {
// 	  os << C[&(* it->neighbor(j))];
// 	  if(CGAL_is_ascii(os)){
// 	    if(j==3) {
// 	      os << endl;
// 	    } else {
// 	      os <<  ' ';
// 	    }
// 	  }
// 	}
// 	++it;
//       }
//       break;
//     }
//   case 2:
//     {
//       os << m;
//       if(CGAL_is_ascii(os)){ os << endl;}

//       // write the facets
//       Facet_iterator it = tds.facets_begin();
//       while( it != tds.facets_end() ) {
// 	C[&*((*it).first)] = i++;
// 	for(j = 0; j < 3; j++){
// 	  os << V[(*it).first->vertex(j)];
// 	  if(CGAL_is_ascii(os)) {
// 	    if ( j==2 ) {
// 	      os << endl;
// 	    } else {
// 	      os <<  ' ';
// 	    }
// 	  }
// 	}
// 	++it;
//       }
//       CGAL_triangulation_assertion( i == m );
      
//       // write the neighbors
//       it = tds.facets_begin();
//       while ( it != tds.facets_end() ) {
// 	for (j = 0; j < 3; j++) {
// 	  os << C[&*((*it).first->neighbor(j))];
// 	  if(CGAL_is_ascii(os)){
// 	    if(j==2) {
// 	      os << endl;
// 	    } else {
// 	      os <<  ' ';
// 	    }
// 	  }
// 	}
// 	++it;
//       }
//       break;
//     }
//   case 1:
//     {
//       os << m;
//       if(CGAL_is_ascii(os)){ os << endl;}

//       // write the edges
//       Edge_iterator it = tds.edges_begin();
//       while( it != tds.edges_end() ) {
// 	C[&*((*it).first)] = i++;
// 	for(j = 0; j < 2; j++){
// 	  os << V[(*it).first->vertex(j)];
// 	  if(CGAL_is_ascii(os)) {
// 	    if ( j==1 ) {
// 	      os << endl;
// 	    } else {
// 	      os <<  ' ';
// 	    }
// 	  }
// 	}
// 	++it;
//       }
//       CGAL_triangulation_assertion( i == m );
      
//       // write the neighbors
//       it = tds.edges_begin();
//       while ( it != tds.edges_end() ) {
// 	for (j = 0; j < 2; j++) {
// 	  os << C[&*((*it).first->neighbor(j))];
// 	  if(CGAL_is_ascii(os)){
// 	    if(j==1) {
// 	      os << endl;
// 	    } else {
// 	      os <<  ' ';
// 	    }
// 	  }
// 	}
// 	++it;
//       }
//       break;
//     }
// //   default:
// //     {
// //       os << m;
// //       if(CGAL_is_ascii(os)){ os << endl;}
// //       break;
// //     }
//   }
  return os;
}

// not documented
template < class Vb, class Cb>
istream& read_cells
(istream& is,
 CGAL_Triangulation_data_structure_3<Vb,Cb>  &tds,
 int n,
 // vector<void*> &V(n),
 map< int, CGAL_Triangulation_data_structure_3<Vb,Cb>::Vertex*, less<int> > &V,
 // vector<void*> &C(m)
 int & m,
 map< int, CGAL_Triangulation_data_structure_3<Vb,Cb>::Cell*, less<int> > &C
)
{
  typedef CGAL_Triangulation_data_structure_3<Vb,Cb> Tds;
  typedef  Tds::Vertex  Vertex;
  typedef  Tds::Cell Cell;
  typedef  Tds::Edge Edge;
  typedef  Tds::Facet Facet;
 
    // creation of the cells and neighbors
  int i;
  switch (tds.dimension()) {
  case 3:
    {
      is >> m;
      Cell* c;

      int i0, i1, i2, i3;
      for(i = 0; i < m; i++) {
	is >> i0 >> i1 >> i2 >> i3;
	c = new Cell(tds, V[i0], V[i1], V[i2], V[i3]);
	C[i] = c;
	V[i0]->set_cell(c);
	V[i1]->set_cell(c);
	V[i2]->set_cell(c);
	V[i3]->set_cell(c);
      }
      for(i = 0; i < m; i++) {
        is >> i0 >> i1 >> i2 >> i3;
        c = C[i];
        c->set_neighbor(0, C[i0]);
        c->set_neighbor(1, C[i1]);
        c->set_neighbor(2, C[i2]);
        c->set_neighbor(3, C[i3]);
      }
      break;
    }
  case 2:
    {
      is >> m;
      Cell* c;

      int i0, i1, i2;
      for(i = 0; i < m; i++) {
	is >> i0 >> i1 >> i2;
	c = new Cell(tds, V[i0], V[i1], V[i2], NULL);
	C[i] = c;
	V[i0]->set_cell(c);
	V[i1]->set_cell(c);
	V[i2]->set_cell(c);
      }
      for(i = 0; i < m; i++) {
        is >> i0 >> i1 >> i2;
	c = C[i];
        c->set_neighbor(0, C[i0]);
        c->set_neighbor(1, C[i1]);
        c->set_neighbor(2, C[i2]);
      }
      break;
    }
  case 1:
    {
      is >> m;
     Cell* c;

      int i0, i1;
      for(i = 0; i < m; i++) {
	is >> i0 >> i1;
	c = new Cell(tds, V[i0], V[i1], NULL, NULL);
	C[i] = c;
	V[i0]->set_cell(c);
	V[i1]->set_cell(c);
      }
      for(i = 0; i < m; i++) {
        is >> i0 >> i1;
	c = C[i];
        c->set_neighbor(0, C[i0]);
        c->set_neighbor(1, C[i1]);
      }
      break;
    }
  case 0:
    {
      m = 2;
      Cell* c;

      CGAL_triangulation_assertion( (n == 2) );
      for (i=0; i < 2; i++) {
	c = new Cell(tds, V[i], NULL, NULL, NULL);
	C[i] = c;
	V[i]->set_cell(c);
      }
      for (i=0; i < 2; i++) {
	c = C[i];
        c->set_neighbor(0, C[1-i]);
      }
      break;
    }
  case -1:
    {
      m = 1;
      Cell* c;
      CGAL_triangulation_assertion( (n == 1) );
      c = new Cell(tds, V[0], NULL, NULL, NULL);
      C[0] = c;
      V[0]->set_cell(c);
      break;
    }
  }
  return is;
}
// not documented
template < class Vb, class Cb>
ostream& print_cells
(ostream& os, 
 const CGAL_Triangulation_data_structure_3<Vb,Cb>  &tds,
 int n,
 map< void*, int, less<void*> > &V
)
{
  typedef CGAL_Triangulation_data_structure_3<Vb,Cb> Tds;
  typedef  Tds::Vertex  Vertex;
  typedef  Tds::Cell Cell;
  typedef  Tds::Edge Edge;
  typedef  Tds::Facet Facet;
  typedef  Tds::Vertex_iterator  Vertex_iterator;
  typedef  Tds::Cell_iterator  Cell_iterator;
  typedef  Tds::Edge_iterator  Edge_iterator;
  typedef  Tds::Facet_iterator  Facet_iterator;

  map< void*, int, less<void*> > C;

  int i = 0;
  int j;
  int m;
  
  switch ( tds.dimension() ) {
  case 3:
    {
      m = tds.number_of_cells();
      os << m;
      if(CGAL_is_ascii(os)){ os << endl;}

      // write the cells
      Cell_iterator it = tds.cells_begin();
      while( it != tds.cells_end() ) {
	C[&(*it)] = i++;
	for(j = 0; j < 4; j++){
	  os << V[it->vertex(j)];
	  if(CGAL_is_ascii(os)) {
	    if ( j==3 ) {
	      os << endl;
	    } else {
	      os << ' ';
	    }
	  }
	}
	++it;
      }
      CGAL_triangulation_assertion( i == m );
      
      // write the neighbors
      it = tds.cells_begin();
      while ( it != tds.cells_end() ) {
	for (j = 0; j < 4; j++) {
	  os << C[&(* it->neighbor(j))];
	  if(CGAL_is_ascii(os)){
	    if(j==3) {
	      os << endl;
	    } else {
	      os <<  ' ';
	    }
	  }
	}
	++it;
      }
      break;
    }
  case 2:
    {
      m = tds.number_of_facets();
      os << m;
      if(CGAL_is_ascii(os)){ os << endl;}

      // write the facets
      Facet_iterator it = tds.facets_begin();
      while( it != tds.facets_end() ) {
	C[&*((*it).first)] = i++;
	for(j = 0; j < 3; j++){
	  os << V[(*it).first->vertex(j)];
	  if(CGAL_is_ascii(os)) {
	    if ( j==2 ) {
	      os << endl;
	    } else {
	      os <<  ' ';
	    }
	  }
	}
	++it;
      }
      CGAL_triangulation_assertion( i == m );
      
      // write the neighbors
      it = tds.facets_begin();
      while ( it != tds.facets_end() ) {
	for (j = 0; j < 3; j++) {
	  os << C[&*((*it).first->neighbor(j))];
	  if(CGAL_is_ascii(os)){
	    if(j==2) {
	      os << endl;
	    } else {
	      os <<  ' ';
	    }
	  }
	}
	++it;
      }
      break;
    }
  case 1:
    {
      m = tds.number_of_edges();
      os << m;
      if(CGAL_is_ascii(os)){ os << endl;}

      // write the edges
      Edge_iterator it = tds.edges_begin();
      while( it != tds.edges_end() ) {
	C[&*((*it).first)] = i++;
	for(j = 0; j < 2; j++){
	  os << V[(*it).first->vertex(j)];
	  if(CGAL_is_ascii(os)) {
	    if ( j==1 ) {
	      os << endl;
	    } else {
	      os <<  ' ';
	    }
	  }
	}
	++it;
      }
      CGAL_triangulation_assertion( i == m );
      
      // write the neighbors
      it = tds.edges_begin();
      while ( it != tds.edges_end() ) {
	for (j = 0; j < 2; j++) {
	  os << C[&*((*it).first->neighbor(j))];
	  if(CGAL_is_ascii(os)){
	    if(j==1) {
	      os << endl;
	    } else {
	      os <<  ' ';
	    }
	  }
	}
	++it;
      }
      break;
    }
//   default:
//     {
//       os << m;
//       if(CGAL_is_ascii(os)){ os << endl;}
//       break;
//     }
  }
  return os;
}
#endif CGAL_TRIANGULATION_DATA_STRUCTURE_3_H
