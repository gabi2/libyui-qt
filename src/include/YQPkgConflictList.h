/*---------------------------------------------------------------------\
|								       |
|		       __   __	  ____ _____ ____		       |
|		       \ \ / /_ _/ ___|_   _|___ \		       |
|			\ V / _` \___ \ | |   __) |		       |
|			 | | (_| |___) || |  / __/		       |
|			 |_|\__,_|____/ |_| |_____|		       |
|								       |
|				core system			       |
|							 (C) SuSE GmbH |
\----------------------------------------------------------------------/

  File:	      YQPkgConflictList.h

  Author:     Stefan Hundhammer <sh@suse.de>

/-*/

// -*- c++ -*-


#ifndef YQPkgConflictList_h
#define YQPkgConflictList_h

#include <y2pm/PkgDep.h>
#include "QY2ListView.h"


class YQPkgConflictAlternative;
class YQPkgConflictResolution;


/**
 * Resolution types for conflicts
 **/
typedef enum YQPkgConflictResolutionType
{
    YQPkgConflictUndo,
    YQPkgConflictIgnore,
    YQPkgConflictBruteForceDelete,
    YQPkgConflictAlternative
};


/**
 * @short Display package dependency conflicts in a tree list and let the user
 * choose how to resolve each conflict.
 **/
class YQPkgConflictList : public QY2ListView
{
    Q_OBJECT

public:
    /**
     * Constructor.
     **/
    YQPkgConflictList( QWidget * parent );

    /**
     * Destructor.
     **/
    virtual ~YQPkgConflictList();

    /**
     * Fill the list with the specified bad list.
     **/
    void fill( PkgDep::ErrorResultList & badList );

    /**
     * Check if the user choices are complete.
     **/
    bool choicesComplete();

public slots:

    /**
     * Activate the choices the user made.
     **/
    void activateUserChoices();


};



/**
 * @short Root item for each individual conflict
 **/
class YQPkgConflict: public QY2ListViewItem
{
public:

    /**
     * Constructor.
     **/
    YQPkgConflict( YQPkgConflictList *		parentList,
		   const PkgDep::ErrorResult &	errorResult );

    /**
     * Destructor.
     **/
    virtual ~YQPkgConflict() {}

    /**
     * Check if this conflict is resolved, i.e. the user made a choice how to
     * deal with this conflict.
     **/
    bool isResolved();

    /**
     * Access the internal ErrorResult.
     **/
    PkgDep::ErrorResult & errorResult() { return _conflict; }

    /**
     * Returns if this conflict needs an alternative from a list.
     **/
    bool needAlternative() { return ! _conflict.alternatives.empty(); }

    /**
     * Returns if this package collides with other packages.
     **/
    bool hasCollisions() { return ! _conflict.conflicts_with.empty(); }

    /**
     * Returns if this package has open (unresolved) requirements.
     **/
    bool hasOpenRequirements() { return ! _conflict.unresolvable.empty(); }


protected:


    /**
     * Format the text line for this item.
     **/
    void formatLine();

    /**
     * Dump all relevant lists from the internal ErrorResult into the conflict
     * list.
     **/
    void dumpLists();

    /**
     * Dump a list of package relations into the conflict list:
     * Create a new list entry for each list entry.
     * If 'header' is non-null, create a bracketing list item with text
     * 'header' and insert the list items below that new item.
     * Splits into a sublist at (about) 'splitThreshold' if this is > 1.
     * Does nothing if the list is empty.
     **/
    void dumpList( QListViewItem * 		parent,
		   PkgDep::RelInfoList & 	list,
		   int				splitThreshold = -1,
		   const QString & 		header	   = QString::null );

    /**
     * Add suggestions how to resolve this conflict.
     **/
    void addResolutionSuggestions();

    /**
     * Add resolution suggestion: Undo what caused this conflict
     * (i.e. don't remove, don't install, ...).
     **/
    void addUndoResolution( QY2CheckListItem * parent );

    /**
     * Add a list of alternatives if there are any.
     **/
    void addAlternativesList( QY2CheckListItem * parent );

    /**
     * Add brute force resolution suggestion: Delete all dependent packages.
     **/
    void addDeleteResolution( QY2CheckListItem * parent );

    /**
     * Add resolution suggestion: Ignore conflict, risk inconsistent system
     **/
    void addIgnoreResolution( QY2CheckListItem * parent );

    /**
     * Dump the 'remove_to_solve_conflict' list into the conflict list.
     **/
    void dumpDeleteList( QListViewItem * parent );
    

    /**
     * Paint method. Reimplemented from @ref QListViewItem a different
     * font can be used.
     *
     * Reimplemented from QY2ListViewItem.
     **/
    virtual void paintCell( QPainter *		painter,
			    const QColorGroup &	colorGroup,
			    int			column,
			    int			width,
			    int			alignment );


    // Data members

    PMObjectPtr			_pmObj;
    QString			_shortName;	// Only pkg name (no version)
    QString			_fullName;	// Name + edition
    PMSelectable::UI_Status	_status;
    PMSelectable::UI_Status	_undo_status;

    bool			_collision;

    YQPkgConflictResolution *	_firstResolution;

    PkgDep::ErrorResult		_conflict;
    YQPkgConflictList *		_parentList;
};



class YQPkgConflictResolution: public QY2CheckListItem
{
public:

    /**
     * Constructor for generic resolutions (not alternatives)
     **/
    YQPkgConflictResolution( QY2CheckListItem *			parent,
			     const QString & 			text,
			     YQPkgConflictResolutionType	type );

    /**
     * Constructor for alternatives
     **/
    YQPkgConflictResolution( QY2CheckListItem *	parent,
			     PMObjectPtr	pmObj );

    /**
     * Returns the type of this resolution.
     **/
    YQPkgConflictResolutionType type() const { return _type; }

    /**
     * Returns the corresponding PMObject.
     **/
    PMObjectPtr pmObj() const { return _pmObj; }

    
protected:

    // Data members

    YQPkgConflictResolutionType	_type;
    PMObjectPtr			_pmObj;
};


#endif // ifndef YQPkgConflictList_h