#include <entities/tag.h>
#include "noteapi.h"

NoteApi* NoteApi::fetch(int id) {
    Note note = Note::fetch(id);

    if (note.isFetched()) {
        this->id = note.getId();
        name = note.getName();
        fileName = note.getFileName();
        noteText = note.getNoteText();
        hasDirtyData = note.getHasDirtyData();
        noteSubFolderId = note.getNoteSubFolderId();
        decryptedNoteText = note.getDecryptedNoteText();
        fileCreated = note.getFileCreated();
        fileLastModified = note.getFileLastModified();
    }

    return this;
}

NoteApi* NoteApi::fromNote(Note note) {
    NoteApi *noteApi = new NoteApi();
    noteApi->fetch(note.getId());
    return noteApi;
}

//NoteApi NoteApi::fromNote(Note note) {
//    NoteApi noteApi;
//    noteApi.fetch(note.getId());
//    return noteApi;
//}

/**
 * Returns all linked tags
 */
QQmlListProperty<TagApi> NoteApi::tags() {
    _tags.clear();

    Note note = Note::fetch(id);
    QList<Tag> tags = Tag::fetchAllOfNote(note);
    QListIterator<Tag> itr(tags);
    while (itr.hasNext()) {
        Tag tag = itr.next();

        TagApi* tagApi = new TagApi();
        tagApi->fetch(tag.getId());
        _tags.append(tagApi);
    }

    return QQmlListProperty<TagApi>(this, _tags);
}

/**
 * Returns the names of all linked tags
 */
QStringList NoteApi::tagNames() const {
    QStringList tagNameList;
    Note note = Note::fetch(id);
    QList<Tag> tags = Tag::fetchAllOfNote(note);
    QListIterator<Tag> itr(tags);
    while (itr.hasNext()) {
        Tag tag = itr.next();
        tagNameList.append(tag.getName());
    }

    return tagNameList;
}

/**
 * Adds a tag to the note
 *
 * @param tagName
 * @return true if the note was tagged
 */
bool NoteApi::addTag(QString tagName) {
    if (tagName.isEmpty()) {
        return false;
    }

    Note note = Note::fetch(id);
    if (!note.exists()) {
        return false;
    }

    // create a new tag if it doesn't exist
    Tag tag = Tag::fetchByName(tagName);
    if (!tag.isFetched()) {
        tag.setName(tagName);
        tag.store();
    }

    return tag.linkToNote(note);
}

/**
 * Removes a tag from the note
 *
 * @param tagName
 * @return true if the tag was removed from the note
 */
bool NoteApi::removeTag(QString tagName) {
    Tag tag = Tag::fetchByName(tagName);
    if (!tag.exists()) {
        return false;
    }

    Note note = Note::fetch(id);
    if (!note.exists()) {
        return false;
    }

    return tag.removeLinkToNote(note);
}

/**
 * Fetches all notes
 * Disclaimer: not tested, might not work yet
 *
 * @param limit
 * @param offset
 * @return
 */
QQmlListProperty<NoteApi> NoteApi::fetchAll(int limit, int offset) {
    QList<int> noteIds = Note::fetchAllIds(limit, offset);
    QList<NoteApi *> notes;

    Q_FOREACH(int noteId, noteIds) {
            NoteApi *note = NoteApi::fetch(id);
            notes.append(note);
        }

    return QQmlListProperty<NoteApi>(this, notes);
}
