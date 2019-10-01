/*
 *  This file is part of the EPICS QT Framework, initially developed at the Australian Synchrotron.
 *
 *  The EPICS QT Framework is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  The EPICS QT Framework is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with the EPICS QT Framework.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright (c) 2012
 *
 *  Author:
 *    Andrew Rhyder
 *  Contact details:
 *    andrew.rhyder@synchrotron.org.au
 */

/** Description:
 *
 * A class to manage user messages.
 *
 * This class passes messages between widgets and application code
 *
 * This class is used as a base class.
 *
 * Messages are sent by calling sendMessage()
 * Messages are received by implementing newMessage() in the derived class.
 *
 * Messages can be filtered based on a source ID or a form ID
 *
 * The derived widget is free to set the source ID to any value
 *
 * Derived form widgets (QEForm) get a unique form ID using getNextMessageFormId()
 * (as well as being able to set a source ID like any other QE widget) and pass
 * this unique form ID to all widgets within the form using the ContainerProfile class.
 *
 * Messages sent by a widget are received by all widgets and can filter the messages
 * required by form id and source id.
 * The form id is under the management of the QEForm widget, the source ID is under
 * the control of the GUI designer.
 *
 * The QEForm widget does not display messages, but re-send them using its own
 * form ID. Read on to see how this can be used.
 *
 * Widgets that generate messages, and widgets (or application code) that uses messages
 * can be set up as follows:
 *
 * - Application wide logging:
 *   An application with a single log window can can base a class on the UserMessage class
 *   and set up filtering to receive all messages.
 *   An application with log messages for seperate windows containing QEForm
 *   widgets (such as ASgui) can base each window class on the UserMessage class, then
 *   set up filtering for the appropriate form ID.
 *
 * - Logging within an QEForm.
 *   A logging widget can be set to filter matching on the current form and so will pick up
 *   messages from any sibling widget. This includes messages from an sibling widget which is
 *   a nested QEForm. Whatever messages that nested form is set to receive, it will resend
 *   to its siblings. For example, if it is set to receive messages from the widgets it
 *   contains, these are resent up one level to the main form.
 *   If messages are dealt with within the nested QEForm (for example, it may have its own
 *   logging widget) then the nested QEForm could be set up not to filter and resend any messages.
 */

#include "UserMessage.h"

// Static variables used to manage message signals and slots.
UserMessageSignal UserMessage::userMessageSignal;
unsigned int UserMessage::nextMessageFormId = 1;


/// Construction
UserMessage::UserMessage()
{
    // Initialise
    formId = 0;
    sourceId = 0;
    formFilter = MESSAGE_FILTER_NONE;
    sourceFilter = MESSAGE_FILTER_ANY;  // Note, default of MESSAGE_FILTER_ANY means default implementation of
                                        // newMessage() will be called which cancels future unwanted messages.
                                        // An alternate default of MESSAGE_FILTER_NONE would mean newMEssage()
                                        // is never called (good), but the opportunity to cancel future signals
                                        // for uninterested widgets would be lost (bad)

    childFormId = 0;

    // Allow the object receiving messages to pass them back to us
    userMessageSlot.setOwner( this );

    // Establish the connection between the common message signaler, and this instance's message slot
    QObject::connect( &userMessageSignal, SIGNAL( message( QString, message_types, unsigned int, unsigned int, UserMessage* ) ),
                      &userMessageSlot, SLOT( message( QString, message_types, unsigned int, unsigned int, UserMessage* ) ) );
}

/// Destruction
UserMessage::~UserMessage()
{
}

// Set the source ID
// (the ID set up by the GUI designer, usually matched to the source ID of logging widgets)
void UserMessage::setSourceId( unsigned int sourceIdIn )
{
    sourceId = sourceIdIn;
}

// Set the form ID (the the same ID for all sibling widgets within an QEForm widget)
void UserMessage::setFormId( unsigned int formIdIn )
{
    formId = formIdIn;
}

// Set the message filtering applied to the form ID
void UserMessage::setFormFilter( message_filter_options formFilterIn )
{
    formFilter = formFilterIn;
}

// Set the message filtering applied to the source ID
void UserMessage::setSourceFilter( message_filter_options sourceFilterIn )
{
    sourceFilter = sourceFilterIn;
}

// Get the source ID (the ID set up by the GUI designer, usually matched to the source ID of logging widgets
unsigned int UserMessage::getSourceId()
{
    return sourceId;
}

// Get the form ID (the the same ID for all sibling widgets within an QEForm widget)
unsigned int UserMessage::getFormId()
{
    return formId;
}

// Get the message filtering applied to the form ID
UserMessage::message_filter_options UserMessage::getFormFilter()
{
    return formFilter;
}

// Get the message filtering applied to the source ID
UserMessage::message_filter_options UserMessage::getSourceFilter()
{
    return sourceFilter;
}

// Set the ID for of all widgets that are children of this widget
void UserMessage::setChildFormId( unsigned int childFormIdIn )
{
    childFormId = childFormIdIn;
}

// Get the ID for of all widgets that are children of this widget
unsigned int UserMessage::getChildFormId()
{
    return childFormId;
}

// Generate a new form ID for all widgets in a new form
unsigned int UserMessage::getNextMessageFormId()
{
    return nextMessageFormId++;
}

// Convenience function to provide string names for each message type
QString UserMessage::getMessageTypeName( message_types type )
{
    switch( type )
    {
        case MESSAGE_TYPE_INFO:    return "Information";
        case MESSAGE_TYPE_WARNING: return "Warning";
        case MESSAGE_TYPE_ERROR:   return "Error";
    }
    return "";
}


// Send a message to the user.
// A string containing the message and a string containing information as to the source of the message is required
void UserMessage::sendMessage( QString message,
                               QString source,
                               message_types type )
{
    // Combine message and source and call base send message function
    sendMessage( QString("%1 (Source %2)").arg(message).arg(source), type );
}

// Send a message to the user.
// A string containing the message is required
void UserMessage::sendMessage( QString msg,
                               message_types type )
{
    // Send the message
    userMessageSignal.sendMessage( msg, type, formId, sourceId, this );
}

// Emit a signal to all other user message classes
// Note, there is only a single UserMessageSignal class instance
void UserMessageSignal::sendMessage( QString msg,
                               message_types type,
                               unsigned int formId,
                               unsigned int sourceId,
                               UserMessage* originator )
{
    emit message( msg, type, formId, sourceId, originator );
}

// Receive a signal from another message class
// Note, there is a UserMessageSlot for every UserMessage class instance
void UserMessageSlot::message( QString msg,
                               message_types type,
                               unsigned int messageFormId,
                               unsigned int messageSourceId,
                               UserMessage* originator )
{
    // Ignore our own messages
    if( originator == owner )
    {
        return;
    }

    // If filter matches, use it.
    if(( owner->formFilter == UserMessage::MESSAGE_FILTER_ANY ) ||                                              // Always match on any form ID
       ( owner->formFilter == UserMessage::MESSAGE_FILTER_MATCH && owner->childFormId == messageFormId ) ||     // Match only on specific form ID
       ( owner->sourceFilter == UserMessage::MESSAGE_FILTER_ANY ) ||                                            // Always match on any source ID
       ( owner->sourceFilter == UserMessage::MESSAGE_FILTER_MATCH && owner->getSourceId() == messageSourceId )) // Match only on specific source ID
    {
        owner->newMessage( msg, type );
    }
}

// Default implementation of virtual function to pass messages to derived classes (typicaly logging widgets or application windows)
// If this default function is called it means the widget is not going to receive any messages and so there is no need to be receiving signals.
// Since only a few widgets will be interested in messages, disconnecting uninterested widgets from message signals will reduce the
// message signal count significantly.
// In summarey if this function is called, there is no re implementation by a derived class, so signals can be disconnected.
void UserMessage::newMessage( QString, message_types )
{
    // Disconnect. If this default implementation of useMessage is called, no derived class is interested in messages
    QObject::disconnect( &userMessageSignal, SIGNAL( message( QString, message_types, unsigned int, unsigned int, UserMessage* ) ),
                         &userMessageSlot, SLOT( message( QString, message_types, unsigned int, unsigned int, UserMessage* ) ) );
}
