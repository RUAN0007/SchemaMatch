#pragma once

#include "IKnowledgeBase.h"

using namespace std;
typedef string URI;

class KB : public IKB {

private:
    string _url;

public:

	/**
	* Returns the total number of types in the knowledge base
	*
	* @return the total number of types in the knowledge base
	*/
	int getNumberOfTypes() ;

	/**
	* Returns the total number of relations in the knowledge base
	*
	* @return the total number of relations in the knowledge base
	*/
	int getNumberOfRelations() ;

	/**
	* Returns all the types in the knowledge base
	*
	* @return a list of URIs, each URI is a type in the knowledge base
	*/
	list<URI> getTypes() ;

	/**
	* Returns all the relations in the knowledge base
	*
	* @return a list of URIs, each URI is a relation in the knowledge base
	*/
	 list<URI> getRelations() ;

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
	 int countType(URI type) ;

	/**
	* Returns the number of instances in the knowledge base
	*
	* @return the number of instances of this type.
	*/
	 int count() ;

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
	 list<URI> listCandidateTypes(string label) ;

	/**
	* List the candidate relations between the subject (label) and the object
	* (label). Note that the returned relations are those only directed from
	* the subject to the object and not the reverse relations.
	*
	* @param subjectLabel
	* @param objectLabel
	* @return
	*/
	 list<URI> listCandidateRelations(string subjectLabel, string objectLabel) ;

	/**
	* List all the entities for the given type.
	*
	* Example: if the type is City, the returned resource are Paris, Rome, ..
	*
	* @param type
	*            the URI of an RDF type
	* @return a list of URIs, each URI is an RDF resource of the given type
	*/
	 list<URI> getEntites(URI type) ;

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
	 bool isValueOfType(URI type, URI value) ;

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
	 bool isRelation(URI subject, URI object, URI relation) ;

	/**
	* Returns all the entities with the given type in the model which are
	* subject in the given relation.
	*
	* @param relation
	* @return
	*/
	 list<URI> getSubjectEntites(URI relation) ;

	/**
	* Returns all the entities with the given type in the model which are
	* object in the given relation.
	*
	* @param relation
	* @return
	*/
	 list<URI> getObjectEntites(URI relation) ;

	/**
	* Returns all the entities with the given type in the model which are
	* subject in the given relation with the given object.
	*
	* @param relation
	* @param subject
	* @return
	*/
	 list<URI> getSubjectEntites(URI relation, URI object) ;

	/**
	* Returns all the entities with the given type in the model which are
	* object in the given relation with the given subject.
	*
	* @param relation
	* @param subject
	* @return
	*/
	 list<URI> getObjectEntites(URI relation, URI subject) ;

	/**
	* Returns the label of the resource in the given language
	*
	* @param resource
	*            the URI of the resource
	* @param langauge
	*            the language code, e.g. "en" for English
	* @return the label of the resource in the given language
	*/
	 string getLabel(URI resource, string langauge) ;

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
	 bool isStringValueOfType(URI type, string label) ;

	// int getShortestPathLength(URI firstType, URI secondtype) ;

	// pair<list<URI>, list<URI>> pathGeneration(URI firstType, URI secondtype) ;

	/**
	*
	* @param subject
	* @param predicate
	* @param object
	*/
	 void insert(URI subject, URI predicate, URI object) ;

	/**
	*
	* @param resource
	* @param newLabel
	*/
	 void updateLabel(URI resource, string newLabel) ;

	 list<string> transform(string inputClass, string outputClass, string inputValue, string description) ;

	 void init(string sparql_endpoint) ;

	 int literalIntSpraqlQuery(string _query);
	 list<URI> uriListSpraqlQuery(string _query);
	 bool askSpraqlQuery(string _query);
	 static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);
	 string generatePossibleValues(string label);
};
