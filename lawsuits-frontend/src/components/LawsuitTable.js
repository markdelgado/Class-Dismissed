import React, { useEffect, useState} from "react";
import { Table, Card, Row, Col, Container } from "react-bootstrap";


const LawsuitTable = () =>{

   const [lawsuits, setLawsuits] = useState([]);
   
    useEffect(() => {
        fetch('/lawsuits.json')
            .then(response => {
                if (!response.ok) {
                    throw new Error(`HTTP error! Status: ${response.status}`);
                }
                return response.json();
            })
            .then(data => setLawsuits(data))
            .catch(error => console.error("Error fetching data:", error));
    }, []);
   
   
    return(
        <Container className="my-4">
    

       
        <Row className='g-4'>
       {lawsuits.map((lawsuit, index)=>(
        <Col key={index} xs={12} sm={6} md={4}>

           <Card style={{ width: '100%' }}>
               <Card.Body>
                   <Card.Title>{lawsuit.title}</Card.Title>

                   <Card.Text>{lawsuit.deadline}</Card.Text>


                   <Card.Link href="#">Another Link</Card.Link>
               </Card.Body>
           </Card>
           </Col>
       ))}
     
        </Row>
        </Container>
);

 
}

export default LawsuitTable;