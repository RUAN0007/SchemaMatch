#pragma once

#include <string>
#include <list>
#include <utility>

using namespace std;
typedef string URI;

class IKB {

public:

	/**
	* Returns the total number of types in the knowledge base
	*
	* @return the total number of types in the knowledge base
	*/
	virtual int getNumberOfTypes() = 0;

	/**
	* Returns the total number of relations in the knowledge base
	*
	* @return the total number of relations in the knowledge base
	*/
	virtual int getNumberOfRelations() = 0;

	/**
	* Returns all the types in the knowledge base
	*
	* @return a list of URIs, each URI is a type in the knowledge base
	*/
	virtual list<URI> getTypes() = 0;

	/**
	* Returns all the relations in the knowledge base
	*
	* @return a list of URIs, each URI is a relation in the knowledge base
	*/
	virtual list<URI> getRelations() = 0;

	/**
	* Returns the number of instances of this type.
	*
	* Example: if type is 'City', the return value is the number of cities in
	* the knowledge base.
	*
	* @param type
	*            the URI of an RDF type
	* @return the number of instances of this type.
	*/
	virtual int countType(URI type) = 0;

	/**
	* Returns the number of instances in the knowledge base
	*
	* @return the number of instances of this type.
	*/
	virtual int count() = 0;

	/**
	* List candidate types for a given resource label.
	*
	* Example: if the label is "Rome", the candidate types can be City,
	* Capital, Location.
	*
	* @param label
	*            a resource label
	* @return the candidate types for the given label
	*/
	virtual list<URI> listCandidateTypes(string label) = 0;

	/**
	* List the candidate relations between the subject (label) and the object
	* (label). Note that the returned relations are those only directed from
	* the subject to the object and not the reverse relations.
	*
	* @param subjectLabel
	* @param objectLabel
	* @return
	*/
	virtual list<URI> listCandidateRelations(string subjectLabel, string objectLabel) = 0;

	/**
	* List all the entities for the given type.
	*
	* Example: if the type is City, the returned resource are Paris, Rome, ..
	*
	* @param type
	*            the URI of an RDF type
	* @return a list of URIs, each URI is an RDF resource of the given type
	*/
	virtual list<URI> getEntites(URI type) = 0;

	/**
	* Checks if the value is of the given type.
	*
	* Example: A resource Italy is of type Country
	*
	* @param type
	*            an RDF type
	* @param value
	*            the URI of the resource
	* @return true if the resource is of this type, otherwise false
	*/
	virtual bool isValueOfType(URI type, URI value) = 0;

	/**
	* Checks if the subject and object are related with the given relation.
	* Uses basic RDFS reasoning only.
	*
	* @param subject
	*            the URI of the subject
	* @param object
	*            the URI of the object
	* @param relation
	*            the relation that is going to be checked
	* @return true if the relation exists between the subject and object
	*/
	virtual bool isRelation(URI subject, URI object, URI relation) = 0;

	/**
	* Returns all the entities with the given type in the model which are
	* subject in the given relation.
	*
	* @param relation
	* @return
	*/
	virtual list<URI> getSubjectEntites(URI relation) = 0;

	/**
	* Returns all the entities with the given type in the model which are
	* object in the given relation.
	*
	* @param relation
	* @return
	*/
	virtual list<URI> getObjectEntites(URI relation) = 0;

	/**
	* Returns all the entities with the given type in the model which are
	* subject in the given relation with the given object.
	*
	* @param relation
	* @param subject
	* @return
	*/
	virtual list<URI> getSubjectEntites(URI relation, URI object) = 0;

	/**
	* Returns all the entities with the given type in the model which are
	* object in the given relation with the given subject.
	*
	* @param relation
	* @param subject
	* @return
	*/
	virtual list<URI> getObjectEntites(URI relation, URI subject) = 0;

	/**
	* Returns the label of the resource in the given language
	*
	* @param resource
	*            the URI of the resource
	* @param langauge
	*            the language code, e.g. "en" for English
	* @return the label of the resource in the given language
	*/
	virtual string getLabel(URI resource, string langauge) = 0;

	/**
	* Checks if the value is of the given type.
	*
	* Example: A resource Italy is of type Country
	*
	* @param type
	*            an RDF type
	* @param value
	*            the label of the resource
	* @return true if the resource is of this type, otherwise false
	*/
	virtual bool isStringValueOfType(URI type, string label) = 0;

	//virtual int getShortestPathLength(URI firstType, URI secondtype) = 0;

	//virtual pair<list<URI>, list<URI>> pathGeneration(URI firstType, URI secondtype) = 0;

	/**
	*
	* @param subject
	* @param predicate
	* @param object
	*/
	virtual void insert(URI subject, URI predicate, URI object) = 0;

	/**
	*
	* @param resource
	* @param newLabel
	*/
	virtual void updateLabel(URI resource, string newLabel) = 0;

	virtual list<string> transform(string inputClass, string outputClass, string inputValue, string description) = 0;

	virtual void init(string sparql_endpoint) = 0;
};
